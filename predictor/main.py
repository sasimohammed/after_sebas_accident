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


# ----------------- COMPRESSION -----------------
def compress_image(image_path, b=2, save_path="compressed.npz", show_images=False):
    img = Image.open(image_path)
    if img.mode == 'RGB':
        arr = np.array(img, dtype=int)
        is_rgb = True
        image_type = "RGB"
    else:
        img = img.convert('L')
        arr = np.array(img, dtype=int)
        is_rgb = False
        image_type = "Grayscale"

    pred = predict_2d(arr)
    diff = arr - pred
    q_diff, deq_diff, mp, minE, maxE, step = quantize_diff(diff, b)
    show_quantization_details(mp, b, minE, maxE, step)

    if is_rgb:
        decompressed = np.zeros_like(arr)
        decompressed[0, :, :] = arr[0, :, :]
        decompressed[:, 0, :] = arr[:, 0, :]
        decompressed[1:, 1:, :] = pred[1:, 1:, :] + deq_diff[1:, 1:, :]
        orig_bits = arr.size * 8
        comp_bits = (arr.shape[0] - 1) * (arr.shape[1] - 1) * arr.shape[2] * b + (arr.shape[0] + arr.shape[1]) * \
                    arr.shape[2] * 8
        np.savez_compressed(save_path,
                            q_diff=q_diff,
                            mp=mp,
                            first_row=arr[0, :, :],
                            first_col=arr[:, 0, :],
                            pred=pred,
                            shape=arr.shape,
                            is_rgb=True)
    else:
        decompressed = np.zeros_like(arr)
        decompressed[0, :] = arr[0, :]
        decompressed[:, 0] = arr[:, 0]
        decompressed[1:, 1:] = pred[1:, 1:] + deq_diff[1:, 1:]
        orig_bits = arr.size * 8
        comp_bits = (arr.shape[0] - 1) * (arr.shape[1] - 1) * b + (arr.shape[0] + arr.shape[1]) * 8
        np.savez_compressed(save_path,
                            q_diff=q_diff,
                            mp=mp,
                            first_row=arr[0, :],
                            first_col=arr[:, 0],
                            pred=pred,
                            shape=arr.shape,
                            is_rgb=False)

    compression_ratio = orig_bits / comp_bits
    print(f"\nCompression done! Saved to '{save_path}'")
    print(f"Image Type: {image_type}")
    print(f"Compression Ratio: {compression_ratio:.4f}")

    # ---------- SHOW IMAGES ----------
    if show_images:
        fig, axes = plt.subplots(2, 3, figsize=(15, 10))
        axes = axes.ravel()
        if is_rgb:
            # FIX: Prepare images properly for RGB display
            # Original, Predicted, Decompressed should be in FULL COLOR
            # Error, Quantized, De-quantized in grayscale

            # 1. Original (RGB - Full Color)
            original_display = np.clip(arr, 0, 255).astype(np.uint8)

            # 2. Predicted (RGB - Full Color)
            predicted_display = np.clip(pred, 0, 255).astype(np.uint8)

            # 3. Error (Grayscale)
            error_display = np.clip(diff + 128, 0, 255).astype(np.uint8)
            # Convert to grayscale for display
            if len(error_display.shape) == 3:
                error_gray = np.mean(error_display, axis=2).astype(np.uint8)
            else:
                error_gray = error_display

            # 4. Quantized (Grayscale)
            quantized_display = np.clip(q_diff * (255 / (2 ** b - 1)), 0, 255).astype(np.uint8)
            if len(quantized_display.shape) == 3:
                quantized_gray = np.mean(quantized_display, axis=2).astype(np.uint8)
            else:
                quantized_gray = quantized_display

            # 5. De-quantized (Grayscale)
            dequantized_display = np.clip(deq_diff + 128, 0, 255).astype(np.uint8)
            if len(dequantized_display.shape) == 3:
                dequantized_gray = np.mean(dequantized_display, axis=2).astype(np.uint8)
            else:
                dequantized_gray = dequantized_display

            # 6. Decompressed (RGB - Full Color)
            decompressed_display = np.clip(decompressed, 0, 255).astype(np.uint8)

            images_rgb = [
                original_display,  # Original (RGB)
                predicted_display,  # Predicted (RGB)
                error_gray,  # Error (Grayscale)
                quantized_gray,  # Quantized (Grayscale)
                dequantized_gray,  # De-quantized (Grayscale)
                decompressed_display  # Decompressed (RGB)
            ]

            titles = ["Original", "Predicted", "Error", "Quantized", "De-quantized", "Decompressed"]

            for ax, img_data, title in zip(axes, images_rgb, titles):
                ax.axis('off')
                ax.set_title(title, fontsize=12, fontweight='bold')

                # FIX: Proper RGB display for color images
                if title in ["Original", "Predicted", "Decompressed"]:
                    # For RGB images, display in full color
                    # Convert to float in range 0-1 for proper matplotlib display
                    if len(img_data.shape) == 3 and img_data.shape[2] == 3:
                        # Normalize to 0-1 range
                        img_normalized = img_data.astype(np.float32) / 255.0
                        ax.imshow(img_normalized)
                    else:
                        # Fallback to grayscale if not 3-channel
                        ax.imshow(img_data, cmap='gray', vmin=0, vmax=255)
                else:
                    # For grayscale images
                    ax.imshow(img_data, cmap='gray', vmin=0, vmax=255)

        else:
            # Grayscale images - everything in grayscale
            images_gray = [arr, pred, diff, q_diff * (255 / (2 ** b - 1)), deq_diff, decompressed]
            titles = ["Original", "Predicted", "Error", "Quantized", "De-quantized", "Decompressed"]
            for ax, img_data, title in zip(axes, images_gray, titles):
                ax.imshow(np.clip(img_data, 0, 255), cmap='gray', vmin=0, vmax=255)
                ax.set_title(title, fontsize=12, fontweight='bold')
                ax.axis('off')

        plt.suptitle(f'Compression Results - Compression Ratio: {compression_ratio:.2f}', fontsize=14,
                     fontweight='bold')
        plt.tight_layout()
        plt.savefig("compression_result.png", dpi=150, bbox_inches='tight')
        plt.close()
        print(f"6 images saved as 'compression_result.png'")

    # Save the original and decompressed images separately for verification
    if is_rgb:
        original_img = Image.fromarray(np.clip(arr, 0, 255).astype(np.uint8))
        decompressed_img = Image.fromarray(np.clip(decompressed, 0, 255).astype(np.uint8))
        original_img.save("original_color_check.png")
        decompressed_img.save("decompressed_color_check.png")
        print("Original and decompressed images saved separately for color verification")


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

    # Show the decompressed image in a separate figure
    plt.figure(figsize=(8, 8))
    if is_rgb:
        img_display = np.clip(decompressed, 0, 255).astype(np.uint8)
        img_normalized = img_display.astype(np.float32) / 255.0
        plt.imshow(img_normalized)
        plt.title('Decompressed Image (RGB)', fontsize=14, fontweight='bold')
    else:
        plt.imshow(np.clip(decompressed, 0, 255), cmap='gray', vmin=0, vmax=255)
        plt.title('Decompressed Image (Grayscale)', fontsize=14, fontweight='bold')
    plt.axis('off')
    plt.tight_layout()
    plt.savefig("decompressed_preview.png", dpi=150, bbox_inches='tight')
    plt.close()
    print("Decompressed image preview saved as 'decompressed_preview.png'")


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