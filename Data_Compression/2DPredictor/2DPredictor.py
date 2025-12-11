import numpy as np
from PIL import Image
import math
import matplotlib.pyplot as plt

# ----------------- FAST POWER -----------------
def fast_power(base, exp):
    result = 1
    while exp > 0:
        if exp & 1:
            result *= base
        base *= base
        exp >>= 1
    return result

# ----------------- COMPRESSION -----------------
def compress_image(image_path, b, save_path="compressed.npz"):
    # Open image and convert to grayscale
    img = Image.open(image_path).convert('L')
    arr = np.array(img, dtype=int)
    h, w = arr.shape
    
    # Predictor array
    pred = np.zeros_like(arr)
    pred[0, :] = arr[0, :]
    pred[:, 0] = arr[:, 0]
    
    # Predictor for interior pixels
    for i in range(1, h):
        for j in range(1, w):
            if arr[i-1, j-1] <= min(arr[i, j-1], arr[i-1, j]):
                pred[i, j] = max(arr[i, j-1], arr[i-1, j])
            elif arr[i-1, j-1] >= max(arr[i, j-1], arr[i-1, j]):
                pred[i, j] = min(arr[i, j-1], arr[i-1, j])
            else:
                pred[i, j] = arr[i, j-1] + arr[i-1, j] - arr[i-1, j-1]

    # Compute difference
    diff = arr - pred
    minE = np.min(diff[1:,1:]) if h>1 and w>1 else 0
    maxE = np.max(diff[1:,1:]) if h>1 and w>1 else 0
    
    target = fast_power(2, b)
    target = max(target, 1)
    
    range_val = maxE - minE
    step = 1.0 if range_val == 0 else range_val / target
    
    # Build quantization map
    mp = {}
    for k in range(target):
        left = minE + k * step
        right = minE + (k+1) * step
        Li = math.ceil(left)
        Ri = math.floor(right)
        if k == target - 1:
            Ri = maxE
        if Ri < Li:
            Ri = Li
        mp[k] = (Li, Ri)
    
    # Quantize difference
    q_diff = np.zeros_like(diff)
    for i in range(1, h):
        for j in range(1, w):
            normalized = (diff[i,j] - minE)/step
            num = int(normalized + 1e-9)
            num = max(0, min(num, target-1))
            q_diff[i,j] = num

    # De-quantized diff
    deq_diff = np.zeros_like(diff)
    for i in range(1,h):
        for j in range(1,w):
            L,R = mp[q_diff[i,j]]
            deq_diff[i,j] = (L + R)//2

    # Reconstruct image
    orig = np.zeros_like(arr)
    orig[0,:] = arr[0,:]
    orig[:,0] = arr[:,0]
    orig[1:,1:] = pred[1:,1:] + deq_diff[1:,1:]

    # Compute compression ratio
    orig_bits = h*w*8
    comp_bits = (h-1)*(w-1)*b + (h+w)*8
    compression_ratio = orig_bits / comp_bits

    # Save compressed data
    np.savez_compressed(save_path,
                        q_diff=q_diff,
                        mp=mp,
                        first_row=arr[0,:],
                        first_col=arr[:,0],
                        pred=pred,
                        shape=arr.shape)
    
    print(f"\nCompression done! Data saved to '{save_path}'")
    print(f"Compression Ratio: {compression_ratio:.2f}\n")

    # Display all 6 images with titles
    fig, axes = plt.subplots(2, 3, figsize=(12,8))
    axes = axes.ravel()

    images = [arr, pred, diff, q_diff*(255/(2**b-1)), deq_diff, orig]
    titles = ["Original", "Predicted", "Error", "Quantized", "De-quantized", "Reconstructed"]

    for ax, img_data, title in zip(axes, images, titles):
        ax.imshow(np.clip(img_data,0,255), cmap='gray', vmin=0, vmax=255)
        ax.set_title(title)
        ax.axis('off')

    plt.tight_layout()
    plt.show()

# ----------------- DECOMPRESSION -----------------
def decompress_image(compressed_path, save_image_path="reconstructed.png"):
    data = np.load(compressed_path, allow_pickle=True)
    q_diff = data['q_diff']
    mp = data['mp'].item()
    first_row = data['first_row']
    first_col = data['first_col']
    pred = data['pred']
    h, w = data['shape']

    # Reconstruct
    orig = np.zeros((h,w), dtype=int)
    orig[0,:] = first_row
    orig[:,0] = first_col
    orig[1:,1:] = pred[1:,1:]

    # De-quantized diff
    deq_diff = np.zeros_like(q_diff)
    for i in range(1,h):
        for j in range(1,w):
            L,R = mp[q_diff[i,j]]
            deq_diff[i,j] = (L + R)//2
            orig[i,j] += deq_diff[i,j]

    # Save reconstructed image
    reconstructed_img = Image.fromarray(np.clip(orig,0,255).astype(np.uint8))
    reconstructed_img.save(save_image_path)
    print(f"Decompression done! Image saved as '{save_image_path}'")
    reconstructed_img.show()

# ----------------- MENU -----------------
def main():
    while True:
        print("\n===== Predictor Quantization Menu =====")
        print("1. Compress an image")
        print("2. Decompress an image")
        print("3. Exit")
        choice = input("Enter your choice (1/2/3): ")

        if choice == "1":
            image_path = input("Enter path to image: ")
            b = int(input("Enter number of quantization bits (e.g., 2-8): "))
            save_path = input("Enter compressed file name (default: compressed.npz): ") or "compressed.npz"
            compress_image(image_path, b, save_path)
        elif choice == "2":
            compressed_path = input("Enter path to compressed file (.npz): ")
            save_image_path = input("Enter reconstructed image name (default: reconstructed.png): ") or "reconstructed.png"
            decompress_image(compressed_path, save_image_path)
        elif choice == "3":
            print("Exiting program.")
            break
        else:
            print("Invalid choice! Try again.")

if __name__ == "__main__":
    main()
