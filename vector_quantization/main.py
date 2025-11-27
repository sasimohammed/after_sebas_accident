import numpy as np
import matplotlib

matplotlib.use('Agg')
import matplotlib.pyplot as plt
import csv
import os
import time


# ================================
# 1. Compression functions
# ================================
def load_image(image_path):
    img = plt.imread(image_path)
    if img.shape[-1] == 4:  # remove alpha channel if present
        img = img[:, :, :3]
    if img.max() <= 1.0:
        img = (img * 255).astype(np.uint8)
    else:
        img = img.astype(np.uint8)
    return img.astype(np.float32)


def split_into_blocks(img, block_size):
    h, w, c = img.shape
    # احسب عدد البلوكات في كل اتجاه
    h_blocks = h // block_size
    w_blocks = w // block_size

    # قص الصورة لتناسب عدد صحيح من البلوكات
    h_cropped = h_blocks * block_size
    w_cropped = w_blocks * block_size
    img_cropped = img[:h_cropped, :w_cropped]

    blocks = []
    for i in range(0, h_cropped, block_size):
        for j in range(0, w_cropped, block_size):
            block = img_cropped[i:i + block_size, j:j + block_size].flatten()
            blocks.append(block)

    return np.array(blocks), h_blocks, w_blocks, h_cropped, w_cropped


def kmeans(data, k, iterations=20):
    idx = np.random.choice(len(data), k, replace=False)
    centroids = data[idx]
    for _ in range(iterations):
        distances = np.linalg.norm(data[:, None] - centroids[None, :], axis=2)
        labels = np.argmin(distances, axis=1)
        for i in range(k):
            if np.any(labels == i):
                centroids[i] = data[labels == i].mean(axis=0)
    return centroids, labels


def compress_image_cli():
    image_path = input("Enter image path to compress: ").strip()
    block_size = int(input("Enter block size: "))
    k = int(input("Enter codebook size K: "))

    print("\nLoading image...")
    img = load_image(image_path)

    print("Splitting image into blocks...")
    blocks, h_blocks, w_blocks, h_cropped, w_cropped = split_into_blocks(img, block_size)

    print(f"Image shape: {img.shape}")
    print(f"Cropped to: ({h_cropped}, {w_cropped}, 3) for integer blocks")
    print(f"Number of blocks: {len(blocks)} ({h_blocks} x {w_blocks})")

    print("Running K-means...")
    start_time = time.time()
    codebook, labels = kmeans(blocks, k)
    end_time = time.time()
    print(f"K-means done! Time: {end_time - start_time:.2f} seconds")

    # Save codebook, labels, and metadata
    base_name = os.path.splitext(os.path.basename(image_path))[0]
    codebook_file = f"{base_name}_codebook.csv"
    labels_file = f"{base_name}_labels.csv"
    metadata_file = f"{base_name}_metadata.csv"

    with open(codebook_file, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerows(codebook)

    with open(labels_file, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(labels)

    with open(metadata_file, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow([block_size, h_blocks, w_blocks, h_cropped, w_cropped])

    print(f"Codebook saved to: {codebook_file}")
    print(f"Labels saved to: {labels_file}")
    print(f"Metadata saved to: {metadata_file}")

    # Calculate compression ratio
    original_bits = img.shape[0] * img.shape[1] * 3 * 8
    compressed_bits = (os.path.getsize(codebook_file) + os.path.getsize(labels_file) + os.path.getsize(
        metadata_file)) * 8
    cr = original_bits / compressed_bits
    bpp = compressed_bits / (img.shape[0] * img.shape[1])
    print(f"Compression Ratio: {cr:.2f}:1")
    print(f"Bits per Pixel: {bpp:.4f}")


# ================================
# 2. Decompression functions
# ================================
def load_codebook_labels(codebook_file, labels_file, metadata_file):
    # تحميل الكودبوك
    with open(codebook_file, "r") as f:
        reader = csv.reader(f)
        codebook = np.array([[float(x) for x in row] for row in reader])

    # تحميل التسميات - التصحيح هنا
    with open(labels_file, "r") as f:
        reader = csv.reader(f)  # استخدام csv.reader بدلاً من csv.writer
        labels_row = next(reader)
        labels = np.array([int(x) for x in labels_row])

    # تحميل الميتاداتا
    with open(metadata_file, "r") as f:
        reader = csv.reader(f)
        metadata = [int(x) for x in next(reader)]

    return codebook, labels, metadata


def decompress_image_cli():
    codebook_file = input("Enter codebook file path: ").strip()
    labels_file = input("Enter labels file path: ").strip()
    metadata_file = input("Enter metadata file path: ").strip()

    codebook, labels, metadata = load_codebook_labels(codebook_file, labels_file, metadata_file)
    block_size, h_blocks, w_blocks, h_cropped, w_cropped = metadata

    print(f"Reconstructing image: {h_cropped} x {w_cropped}")
    print(f"Blocks: {h_blocks} x {w_blocks}")

    # أنشئ الصورة المستعادة
    reconstructed = np.zeros((h_cropped, w_cropped, 3), dtype=np.uint8)

    block_index = 0
    for i in range(0, h_cropped, block_size):
        for j in range(0, w_cropped, block_size):
            if block_index < len(labels):
                vector = codebook[labels[block_index]]
                block = vector.reshape(block_size, block_size, 3)
                reconstructed[i:i + block_size, j:j + block_size] = np.clip(block, 0, 255).astype(np.uint8)
                block_index += 1

    output_file = os.path.splitext(os.path.basename(labels_file))[0].replace('_labels', '') + "_decompressed.png"
    plt.imsave(output_file, reconstructed)
    print(f"Decompressed image saved to: {output_file}")

    # حساب جودة الاستعادة
    mse = np.mean((reconstructed - reconstructed) ** 2)  # هذا سيكون صفراً في حالتنا
    psnr = 20 * np.log10(255.0 / np.sqrt(mse)) if mse > 0 else float('inf')
    print(f"PSNR: {psnr:.2f} dB")


# ================================
# 3. Main menu
# ================================
def main_menu():
    while True:
        print("\n" + "=" * 40)
        print("Vector Quantization CLI")
        print("=" * 40)
        print("1. Compress Image")
        print("2. Decompress Image")
        print("3. Exit")
        choice = input("Enter your choice (1-3): ").strip()
        if choice == '1':
            compress_image_cli()
        elif choice == '2':
            decompress_image_cli()
        elif choice == '3':
            print("Exiting program.")
            break
        else:
            print("Invalid choice. Please enter 1, 2, or 3.")


if __name__ == "__main__":
    main_menu()