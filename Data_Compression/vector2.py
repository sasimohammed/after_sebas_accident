import numpy as np
import matplotlib.pyplot as plt
import csv

# ===========================
# 1. Load compressed data
# ===========================
# Load codebook
with open("codebook.csv", "r") as f:
    reader = csv.reader(f)
    codebook = np.array([[float(x) for x in row] for row in reader])

# Load labels
with open("labels.csv", "r") as f:
    reader = csv.reader(f)
    labels = np.array([int(x) for x in next(reader)])

print("Loaded codebook and labels.")
print("Codebook shape:", codebook.shape)
print("Labels length:", len(labels))

# ===========================
# 2. Set image dimensions and block size
# ===========================
# You must know these from the compression step
block_size = 2
# Calculate image height and width
num_blocks = len(labels)
blocks_per_row = int(np.sqrt(num_blocks))  # assumes square-ish image
h = blocks_per_row * block_size
w = blocks_per_row * block_size
c = 3

# ===========================
# 3. Reconstruct the image
# ===========================
reconstructed = np.zeros((h, w, c), dtype=np.uint8)
block_index = 0

for i in range(0, h, block_size):
    for j in range(0, w, block_size):
        vector = codebook[labels[block_index]]
        block = vector.reshape(block_size, block_size, c)
        reconstructed[i:i+block_size, j:j+block_size] = block.astype(np.uint8)
        block_index += 1

# ===========================
# 4. Show reconstructed image
# ===========================
plt.imshow(reconstructed)
plt.title("Decompressed Image")
plt.axis("off")
plt.show()
