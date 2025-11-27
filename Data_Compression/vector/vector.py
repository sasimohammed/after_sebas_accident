import numpy as np
import matplotlib.pyplot as plt
import csv

# ========================================================
# 1. Load Image using Matplotlib
# ========================================================
image_path = "8.png"
img = plt.imread(image_path)

# Remove alpha channel if present
if img.shape[-1] == 4:
    img = img[:, :, :3]

# Convert to 0–255 uint8 if needed
if img.max() <= 1.0:
    img = (img * 255).astype(np.uint8)
else:
    img = img.astype(np.uint8)

# Display original image
plt.imshow(img)
plt.title("Original Image")
plt.axis("off")
plt.show()

# Convert to float for computation
img = img.astype(np.float32)

# ========================================================
# 2. Split into blocks (2×2)
# ========================================================
block_size = 2
h, w, c = img.shape
blocks = []

for i in range(0, h - block_size + 1, block_size):
    for j in range(0, w - block_size + 1, block_size):
        block = img[i:i+block_size, j:j+block_size].flatten()
        blocks.append(block)

blocks = np.array(blocks)
print("Blocks shape =", blocks.shape)   # (num_blocks, vector_length)

# ========================================================
# 3. K-means clustering using NumPy
# ========================================================
def kmeans(data, k, iterations=20):
    # Randomly choose k vectors as initial centroids
    idx = np.random.choice(len(data), k, replace=False)
    centroids = data[idx]

    for _ in range(iterations):
        # Compute distances from each vector to each centroid
        distances = np.linalg.norm(data[:, None] - centroids[None, :], axis=2)
        labels = np.argmin(distances, axis=1)

        # Recompute centroids
        for i in range(k):
            if np.any(labels == i):
                centroids[i] = data[labels == i].mean(axis=0)

    return centroids, labels

k = 64  # number of codebook vectors
print("Running K-means...")
codebook, labels = kmeans(blocks, k)
print("K-means done!")
print("Codebook shape =", codebook.shape)

# ========================================================
# 4. Save Codebook + Labels using CSV
# ========================================================
# Save codebook
with open("codebook.csv", "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerows(codebook)

# Save labels
with open("labels.csv", "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(labels)

print("Saved codebook.csv and labels.csv successfully.")


