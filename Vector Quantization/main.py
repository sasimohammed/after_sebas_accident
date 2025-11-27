import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import csv
import os
import customtkinter as ctk
from tkinter import filedialog
from PIL import Image, ImageTk
matplotlib.use('Agg')


# ================================
# 1. Image functions
# ================================
def load_image(image_path):
    img = plt.imread(image_path)
    if img.ndim == 3 and img.shape[-1] == 4:
        img = img[:, :, :3]
    if img.max() <= 1.0:
        img = (img * 255).astype(np.uint8)
    else:
        img = img.astype(np.uint8)
    return img.astype(np.float32)


def pad_image(img, block_size):
    h, w, c = img.shape
    pad_h = (block_size - (h % block_size)) % block_size
    pad_w = (block_size - (w % block_size)) % block_size
    padded_img = np.pad(img, ((0, pad_h), (0, pad_w), (0, 0)), mode='constant')
    return padded_img, pad_h, pad_w


def split_into_blocks(img, block_size):
    padded_img, pad_h, pad_w = pad_image(img, block_size)
    h, w, c = padded_img.shape
    h_blocks = h // block_size
    w_blocks = w // block_size
    blocks = []
    for i in range(0, h, block_size):
        for j in range(0, w, block_size):
            block = padded_img[i:i + block_size, j:j + block_size].flatten()
            blocks.append(block)
    return np.array(blocks), h_blocks, w_blocks, padded_img, pad_h, pad_w


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


def compute_compression_ratio(img, codebook_file, labels_file, metadata_file):
    original_bits = img.shape[0] * img.shape[1] * 3 * 8
    compressed_bits = (
                              os.path.getsize(codebook_file) +
                              os.path.getsize(labels_file) +
                              os.path.getsize(metadata_file)
                      ) * 8
    cr = original_bits / compressed_bits
    bpp = compressed_bits / (img.shape[0] * img.shape[1])
    return cr, bpp


def load_codebook_labels(codebook_file, labels_file, metadata_file):
    codebook = np.array([[float(x) for x in row] for row in csv.reader(open(codebook_file))])
    labels = np.array([int(x) for x in next(csv.reader(open(labels_file)))])
    block_size, h_blocks, w_blocks, pad_h, pad_w = [int(x) for x in next(csv.reader(open(metadata_file)))]
    return codebook, labels, block_size, h_blocks, w_blocks, pad_h, pad_w


def reconstruct_from_codebook(codebook, labels, block_size, h_blocks, w_blocks):
    h = h_blocks * block_size
    w = w_blocks * block_size
    reconstructed = np.zeros((h, w, 3), dtype=np.uint8)
    idx = 0
    for i in range(0, h, block_size):
        for j in range(0, w, block_size):
            vector = codebook[labels[idx]].reshape(block_size, block_size, 3)
            reconstructed[i:i + block_size, j:j + block_size] = np.clip(vector, 0, 255)
            idx += 1
    return reconstructed


# ================================
# 2. GUI App
# ================================
class VQApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("dark-blue")
        self.title("Vector Quantization GUI")
        self.geometry("1200x750")

        self.original_image = None
        self.compressed_preview = None
        self.decompressed_image = None

        self.block_size = ctk.StringVar(value="2")
        self.k_size = ctk.StringVar(value="4")

        self.create_widgets()

    def create_widgets(self):
        # Control panel
        control_frame = ctk.CTkFrame(self, width=300)
        control_frame.pack(side="left", fill="y", padx=10, pady=10)

        preview_frame = ctk.CTkFrame(self)
        preview_frame.pack(side="right", fill="both", expand=True, padx=10, pady=10)

        # Buttons and inputs
        ctk.CTkButton(control_frame, text="Load Original Image", fg_color="#800080", command=self.load_image_gui).pack(
            pady=10)
        ctk.CTkLabel(control_frame, text="Block Size:").pack(pady=5)
        self.block_size_entry = ctk.CTkEntry(control_frame, textvariable=self.block_size)
        self.block_size_entry.pack(pady=5)
        ctk.CTkLabel(control_frame, text="Codebook Size (K):").pack(pady=5)
        self.k_size_entry = ctk.CTkEntry(control_frame, textvariable=self.k_size)
        self.k_size_entry.pack(pady=5)
        ctk.CTkButton(control_frame, text="Compress Image", fg_color="#800080", command=self.compress_image_gui).pack(
            pady=10)
        ctk.CTkButton(control_frame, text="Decompress Image", fg_color="#800080",
                      command=self.decompress_image_gui).pack(pady=10)


        # Compression ratio label
        self.cr_label = ctk.CTkLabel(control_frame, text="Compression Ratio: ")
        self.cr_label.pack(pady=10)

        # Image display frames
        self.img_frame = ctk.CTkFrame(preview_frame)
        self.img_frame.pack(fill="both", expand=True)

        self.orig_label = ctk.CTkLabel(self.img_frame, text="Original Image", anchor="center")
        self.orig_label.grid(row=0, column=0, padx=5, pady=5)
        self.comp_label = ctk.CTkLabel(self.img_frame, text="Compressed Image", anchor="center")
        self.comp_label.grid(row=0, column=1, padx=5, pady=5)
        self.decomp_label = ctk.CTkLabel(self.img_frame, text="Decompressed Image", anchor="center")
        self.decomp_label.grid(row=0, column=2, padx=5, pady=5)

        self.orig_canvas = ctk.CTkLabel(self.img_frame, text="")
        self.orig_canvas.grid(row=1, column=0, padx=5, pady=5)
        self.comp_canvas = ctk.CTkLabel(self.img_frame,text="")
        self.comp_canvas.grid(row=1, column=1, padx=5, pady=5)
        self.decomp_canvas = ctk.CTkLabel(self.img_frame,text="")
        self.decomp_canvas.grid(row=1, column=2, padx=5, pady=5)

    def display_image(self, img_array, canvas):
        img_uint8 = np.clip(img_array, 0, 255).astype(np.uint8)
        im = Image.fromarray(img_uint8)
        im.thumbnail((350, 350))
        imgtk = ImageTk.PhotoImage(im)
        canvas.configure(image=imgtk)
        canvas.image = imgtk

    def load_image_gui(self):
        file_path = filedialog.askopenfilename(filetypes=[("Image Files", "*.png;*.jpg;*.jpeg")])
        if file_path:
            self.original_image = load_image(file_path)
            self.image_path = file_path
            self.display_image(self.original_image, self.orig_canvas)

    def compress_image_gui(self):
        if self.original_image is None:
            return
        block_size = int(self.block_size.get())
        k = int(self.k_size.get())
        blocks, h_blocks, w_blocks, padded_img, pad_h, pad_w = split_into_blocks(self.original_image, block_size)
        codebook, labels = kmeans(blocks, k)

        # Save files
        base = os.path.splitext(os.path.basename(self.image_path))[0]
        self.codebook_file = f"{base}_codebook.csv"
        self.labels_file = f"{base}_labels.csv"
        self.metadata_file = f"{base}_metadata.csv"
        with open(self.codebook_file, "w", newline="") as f:
            csv.writer(f).writerows(codebook)
        with open(self.labels_file, "w", newline="") as f:
            csv.writer(f).writerow(labels)
        with open(self.metadata_file, "w", newline="") as f:
            csv.writer(f).writerow([block_size, h_blocks, w_blocks, pad_h, pad_w])

        self.compressed_preview = reconstruct_from_codebook(codebook, labels, block_size, h_blocks, w_blocks)
        self.display_image(self.compressed_preview, self.comp_canvas)

        # Compression ratio
        cr, bpp = compute_compression_ratio(self.original_image, self.codebook_file, self.labels_file,
                                            self.metadata_file)
        self.cr_label.configure(text=f"Compression Ratio: {cr:.2f}:1 | Bits per Pixel: {bpp:.4f}")

    def decompress_image_gui(self):
        if not hasattr(self, 'codebook_file'):
            return
        codebook, labels, block_size, h_blocks, w_blocks, pad_h, pad_w = load_codebook_labels(
            self.codebook_file, self.labels_file, self.metadata_file
        )
        decompressed = reconstruct_from_codebook(codebook, labels, block_size, h_blocks, w_blocks)
        if pad_h > 0:
            decompressed = decompressed[:-pad_h, :]
        if pad_w > 0:
            decompressed = decompressed[:, :-pad_w]
        self.decompressed_image = decompressed
        self.display_image(decompressed, self.decomp_canvas)


if __name__ == "__main__":
    app = VQApp()
    app.mainloop()
