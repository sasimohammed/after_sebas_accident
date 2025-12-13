import numpy as np
from PIL import Image
import math
import matplotlib
import matplotlib.pyplot as plt

matplotlib.use("Agg")


# ----------------- FAST POWER -----------------
def fast_power(base, exp):
    result = 1
    while exp > 0:
        if exp & 1:
            result *= base
        base *= base
        exp >>= 1
    return result


# ----------------- 2D PREDICTOR -----------------
def predict_2d(arr):
    if len(arr.shape) == 3:  # RGB image
        h, w, c = arr.shape
        pred = np.zeros_like(arr)
        pred[0, :, :] = arr[0, :, :]
        pred[:, 0, :] = arr[:, 0, :]

        for i in range(1, h):
            for j in range(1, w):
                for ch in range(c):
                    a = arr[i, j - 1, ch]
                    b = arr[i - 1, j, ch]
                    c_val = arr[i - 1, j - 1, ch]
                    if c_val <= min(a, b):
                        pred[i, j, ch] = max(a, b)
                    elif c_val >= max(a, b):
                        pred[i, j, ch] = min(a, b)
                    else:
                        pred[i, j, ch] = a + b - c_val
        return pred
    else:  # Grayscale image
        h, w = arr.shape
        pred = np.zeros_like(arr)
        pred[0, :] = arr[0, :]
        pred[:, 0] = arr[:, 0]

        for i in range(1, h):
            for j in range(1, w):
                a = arr[i, j - 1]
                b = arr[i - 1, j]
                c_val = arr[i - 1, j - 1]
                if c_val <= min(a, b):
                    pred[i, j] = max(a, b)
                elif c_val >= max(a, b):
                    pred[i, j] = min(a, b)
                else:
                    pred[i, j] = a + b - c_val
        return pred


# ----------------- QUANTIZATION DETAILS DISPLAY -----------------
def show_quantization_details(mp, b, minE, maxE, step):
    print(f"\nQuantization Details:")
    print(f"Step size: {step:.4f}")
    print(f"Range: [{minE}, {maxE}]")
    print("\nLevel\tBinary\tRange\t\tDe-quantized Value")
    print("-" * 60)

    for level in sorted(mp.keys()):
        L, R = mp[level]
        binary_code = format(level, f'0{b}b')
        deq_value = (L + R) // 2
        range_str = f"[{L}, {R}]"
        print(f"{level}\t{binary_code}\t{range_str}\t{deq_value}")


# ----------------- QUANTIZATION -----------------
def quantize_diff(diff, b):
    if len(diff.shape) == 3:  # RGB image
        h, w, c = diff.shape
        minE = np.min(diff[1:, 1:, :]) if h > 1 and w > 1 else 0
        maxE = np.max(diff[1:, 1:, :]) if h > 1 and w > 1 else 0
    else:  # Grayscale image
        h, w = diff.shape
        minE = np.min(diff[1:, 1:]) if h > 1 and w > 1 else 0
        maxE = np.max(diff[1:, 1:]) if h > 1 and w > 1 else 0

    target = max(fast_power(2, b), 1)
    range_val = maxE - minE
    step = 1.0 if range_val == 0 else range_val / target

    mp = {}
    for k in range(target):
        left = minE + k * step
        right = minE + (k + 1) * step
        Li = math.ceil(left)
        Ri = math.floor(right)
        if k == target - 1:
            Ri = maxE
        if Ri < Li:
            Ri = Li
        mp[k] = (Li, Ri)

    if len(diff.shape) == 3:  # RGB
        q_diff = np.zeros_like(diff)
        deq_diff = np.zeros_like(diff)
        for ch in range(c):
            for i in range(1, h):
                for j in range(1, w):
                    normalized = (diff[i, j, ch] - minE) / step
                    num = int(normalized + 1e-9)
                    num = max(0, min(num, target - 1))
                    q_diff[i, j, ch] = num
                    L, R = mp[num]
                    deq_diff[i, j, ch] = (L + R) // 2
    else:
        q_diff = np.zeros_like(diff)
        for i in range(1, h):
            for j in range(1, w):
                normalized = (diff[i, j] - minE) / step
                num = int(normalized + 1e-9)
                num = max(0, min(num, target - 1))
                q_diff[i, j] = num
        deq_diff = np.zeros_like(diff)
        for i in range(1, h):
            for j in range(1, w):
                L, R = mp[q_diff[i, j]]
                deq_diff[i, j] = (L + R) // 2

    return q_diff, deq_diff, mp, minE, maxE, step


# ----------------- HELPER TO ENSURE RGB -----------------
def ensure_rgb(img):
    if len(img.shape) == 2:  # grayscale
        return np.stack([img]*3, axis=-1)
    return img  # already RGB


# ----------------- COMPRESSION -----------------
def compress_image(image_path, b=2, save_path="compressed.npz", show_images=False):
    img = Image.open(image_path)
    original_array = np.array(img, dtype=np.uint8)

    if img.mode != 'RGB':
        img = img.convert('RGB')
    arr = np.array(img, dtype=int)
    is_rgb = True
    image_type = "RGB"

    pred = predict_2d(arr)
    diff = arr - pred
    q_diff, deq_diff, mp, minE, maxE, step = quantize_diff(diff, b)
    show_quantization_details(mp, b, minE, maxE, step)

    decompressed = np.zeros_like(arr)
    decompressed[0, :, :] = arr[0, :, :]
    decompressed[:, 0, :] = arr[:, 0, :]
    decompressed[1:, 1:, :] = pred[1:, 1:, :] + deq_diff[1:, 1:, :]

    orig_bits = arr.size * 8
    comp_bits = (arr.shape[0] - 1) * (arr.shape[1] - 1) * arr.shape[2] * b + (arr.shape[0] + arr.shape[1]) * arr.shape[2] * 8

    np.savez_compressed(save_path,
                        q_diff=q_diff,
                        mp=mp,
                        first_row=arr[0, :, :],
                        first_col=arr[:, 0, :],
                        pred=pred,
                        shape=arr.shape,
                        is_rgb=True)

    compression_ratio = orig_bits / comp_bits
    print(f"\nCompression done! Saved to '{save_path}'")
    print(f"Image Type: {image_type}")
    print(f"Compression Ratio: {compression_ratio:.4f}")

    if show_images:
        fig, axes = plt.subplots(2, 3, figsize=(15, 10))
        axes = axes.ravel()

        # PREPARE DISPLAY IMAGES
        original_display = ensure_rgb(original_array)
        predicted_display = np.clip(pred, 0, 255).astype(np.uint8)
        error_gray = np.mean(np.clip(diff + 128, 0, 255).astype(np.uint8), axis=2).astype(np.uint8)
        quantized_scaled = np.clip(q_diff[:, :, 0] * (255 / max(1, (2 ** b - 1))), 0, 255).astype(np.uint8)
        dequantized_scaled = np.clip(np.mean(deq_diff + 128, axis=2), 0, 255).astype(np.uint8)
        decompressed_display = np.clip(decompressed, 0, 255).astype(np.uint8)

        display_images = [
            original_display, predicted_display, error_gray,
            quantized_scaled, dequantized_scaled, decompressed_display
        ]
        titles = ["Original", "Predicted", "Error", "Quantized", "De-quantized", "Decompressed"]

        for ax, img_data, title in zip(axes, display_images, titles):
            ax.axis('off')
            ax.set_title(title, fontsize=12, fontweight='bold')
            if title in ["Original", "Predicted", "Decompressed"]:
                ax.imshow(img_data)  # force color
            else:
                ax.imshow(img_data, cmap='gray', vmin=0, vmax=255)

        plt.suptitle(f'Compression Results - Compression Ratio: {compression_ratio:.2f}', fontsize=14, fontweight='bold')
        plt.tight_layout()
        plt.savefig("compression_result.png", dpi=150, bbox_inches='tight')
        plt.close()
        print("✓ 6-image grid saved as 'compression_result.png'")


# ----------------- DECOMPRESSION -----------------
def decompress_image(compressed_path, save_image_path="reconstructed.png"):
    data = np.load(compressed_path, allow_pickle=True)
    q_diff = data['q_diff']
    mp = data['mp'].item()
    first_row = data['first_row']
    first_col = data['first_col']
    pred = data['pred']
    shape = data['shape']
    is_rgb = data['is_rgb'] if 'is_rgb' in data else len(shape) == 3

    if is_rgb:
        h, w, c = shape
        decompressed = np.zeros((h, w, c), dtype=int)
        decompressed[0, :, :] = first_row
        decompressed[:, 0, :] = first_col
        decompressed[1:, 1:, :] = pred[1:, 1:, :]
        deq_diff = np.zeros_like(q_diff)
        for ch in range(c):
            for i in range(1, h):
                for j in range(1, w):
                    L, R = mp[q_diff[i, j, ch]]
                    deq_diff[i, j, ch] = (L + R) // 2
                    decompressed[i, j, ch] += deq_diff[i, j, ch]
        image_type = "RGB"
    else:
        h, w = shape
        decompressed = np.zeros((h, w), dtype=int)
        decompressed[0, :] = first_row
        decompressed[:, 0] = first_col
        decompressed[1:, 1:] = pred[1:, 1:]
        deq_diff = np.zeros_like(q_diff)
        for i in range(1, h):
            for j in range(1, w):
                L, R = mp[q_diff[i, j]]
                deq_diff[i, j] = (L + R) // 2
                decompressed[i, j] += deq_diff[i, j]
        image_type = "Grayscale"

    reconstructed_img = Image.fromarray(np.clip(decompressed, 0, 255).astype(np.uint8))
    reconstructed_img.save(save_image_path)
    print(f"\nDecompression done! Saved as '{save_image_path}'")
    print(f"Image Type: {image_type}")
    print(f"Image dimensions: {w}x{h}")


# ----------------- MENU -----------------
def main():
    while True:
        print("\n" + "=" * 60)
        print("PREDICTOR QUANTIZATION MENU")
        print("=" * 60)
        print("1. Compress an image")
        print("2. Decompress an image")
        print("3. Exit")
        print("-" * 60)
        choice = input("Enter your choice (1/2/3): ")

        if choice == "1":
            print("\n" + "-" * 60)
            print("IMAGE COMPRESSION")
            print("-" * 60)
            image_path = input("Enter path to image: ")
            bits_input = input("Enter quantization bits (default=2): ").strip()
            b = 2 if bits_input == "" else int(bits_input)
            save_path = input("Enter output filename (default: compressed.npz): ") or "compressed.npz"

            compress_image(image_path, b, save_path, show_images=True)
            print("\nReturning to menu...\n")

        elif choice == "2":
            print("\n" + "-" * 60)
            print("IMAGE DECOMPRESSION")
            print("-" * 60)
            compressed_path = input("Enter path to .npz file: ")
            save_image_path = input("Enter output image name (default: reconstructed.png): ") or "reconstructed.png"

            decompress_image(compressed_path, save_image_path)
            print("\nReturning to menu...\n")

        elif choice == "3":
            print("\nExiting program.")
            break

        else:
            print("Invalid choice! Try again.")


if __name__ == "__main__":
    main()
