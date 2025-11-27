import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import csv
import os
import customtkinter as ctk
from tkinter import filedialog, messagebox
from PIL import Image, ImageTk

matplotlib.use('Agg')

def load_image(image_path):
    try:
        img = plt.imread(image_path)
        if img.ndim == 3 and img.shape[-1] == 4:
            img = img[:, :, :3]
        if img.max() <= 1.0:
            img = (img * 255).astype(np.uint8)
        else:
            img = img.astype(np.uint8)
        return img.astype(np.float32)
    except Exception as e:
        raise Exception(f"Error loading image: {str(e)}")

def pad_image(img, block_size):
    h, w, c = img.shape
    pad_h = (block_size - (h % block_size)) % block_size
    pad_w = (block_size - (w % block_size)) % block_size
    padded_img = np.pad(img, ((0, pad_h), (0, pad_w), (0, 0)), mode='edge')
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
    if len(data) < k:
        k = len(data)
    idx = np.random.choice(len(data), k, replace=False)
    centroids = data[idx].copy()
    for _ in range(iterations):
        distances = np.linalg.norm(data[:, None] - centroids[None, :], axis=2)
        labels = np.argmin(distances, axis=1)
        new_centroids = np.array([data[labels == i].mean(axis=0) if np.any(labels == i) else centroids[i]
                                  for i in range(k)])
        if np.allclose(centroids, new_centroids):
            break
        centroids = new_centroids
    return centroids, labels

def compute_compression_ratio(img, codebook_file, labels_file, metadata_file):
    original_bits = img.shape[0] * img.shape[1] * 3 * 8
    compressed_bits = (
                              os.path.getsize(codebook_file) +
                              os.path.getsize(labels_file) +
                              os.path.getsize(metadata_file)
                      ) * 8
    cr = original_bits / compressed_bits if compressed_bits > 0 else 0
    bpp = compressed_bits / (img.shape[0] * img.shape[1]) if img.size > 0 else 0
    return cr, bpp

def load_codebook_labels(codebook_file, labels_file, metadata_file):
    codebook = np.array([[float(x) for x in row] for row in csv.reader(open(codebook_file))])
    labels = np.array([int(x) for x in next(csv.reader(open(labels_file)))])
    metadata = [int(x) for x in next(csv.reader(open(metadata_file)))]
    return codebook, labels, *metadata

def reconstruct_from_codebook(codebook, labels, block_size, h_blocks, w_blocks):
    h = h_blocks * block_size
    w = w_blocks * block_size
    reconstructed = np.zeros((h, w, 3), dtype=np.uint8)
    idx = 0
    for i in range(0, h, block_size):
        for j in range(0, w, block_size):
            if idx < len(labels):
                vector = codebook[labels[idx]].reshape(block_size, block_size, 3)
                reconstructed[i:i + block_size, j:j + block_size] = np.clip(vector, 0, 255).astype(np.uint8)
                idx += 1
    return reconstructed

class VQApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("dark-blue")
        self.title("Image Vector Quantization")
        self.geometry("1300x800")
        self.minsize(1200, 700)

        self.original_image = None
        self.compressed_preview = None
        self.decompressed_image = None
        self.image_path = None

        self.block_size = ctk.StringVar(value="8")
        self.k_size = ctk.StringVar(value="16")

        self.create_widgets()

    def create_widgets(self):
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)

        sidebar = ctk.CTkFrame(self, width=280, corner_radius=0)
        sidebar.grid(row=0, column=0, sticky="nsew", padx=(0, 5))
        sidebar.grid_propagate(False)

        main_content = ctk.CTkFrame(self, corner_radius=0)
        main_content.grid(row=0, column=1, sticky="nsew", padx=(5, 0))
        main_content.grid_columnconfigure(0, weight=1)
        main_content.grid_rowconfigure(1, weight=1)

        title_frame = ctk.CTkFrame(sidebar, fg_color="transparent")
        title_frame.pack(fill="x", padx=20, pady=20)

        ctk.CTkLabel(title_frame, text="VQ Compressor",
                     font=("Arial", 20, "bold")).pack(pady=(0, 5))
        ctk.CTkLabel(title_frame, text="Vector Quantization Tool",
                     font=("Arial", 12), text_color="gray").pack()

        file_section = ctk.CTkFrame(sidebar)
        file_section.pack(fill="x", padx=15, pady=(0, 15))

        ctk.CTkLabel(file_section, text="Image File",
                     font=("Arial", 14, "bold")).pack(pady=(10, 5))

        self.load_btn = ctk.CTkButton(file_section, text="📁 Load Image",
                                      command=self.load_image_gui,
                                      height=35,fg_color="#800080",hover_color="#5a005a")
        self.load_btn.pack(fill="x", padx=10, pady=5)

        settings_section = ctk.CTkFrame(sidebar)
        settings_section.pack(fill="x", padx=15, pady=(0, 15))

        ctk.CTkLabel(settings_section, text="Compression Settings",
                     font=("Arial", 14, "bold")).pack(pady=(10, 10))

        ctk.CTkLabel(settings_section, text="Block Size:").pack(anchor="w", padx=10)
        block_frame = ctk.CTkFrame(settings_section, fg_color="transparent")
        block_frame.pack(fill="x", padx=10, pady=(5, 10))
        self.block_size_entry = ctk.CTkEntry(block_frame, textvariable=self.block_size,
                                             width=80, height=35)
        self.block_size_entry.pack(side="left")
        ctk.CTkLabel(block_frame, text="pixels", text_color="gray").pack(side="left", padx=(10, 0))

        ctk.CTkLabel(settings_section, text="Codebook Size (K):").pack(anchor="w", padx=10)
        k_frame = ctk.CTkFrame(settings_section, fg_color="transparent")
        k_frame.pack(fill="x", padx=10, pady=(5, 10))
        self.k_size_entry = ctk.CTkEntry(k_frame, textvariable=self.k_size,
                                         width=80, height=35)
        self.k_size_entry.pack(side="left")
        ctk.CTkLabel(k_frame, text="vectors", text_color="gray").pack(side="left", padx=(10, 0))

        action_section = ctk.CTkFrame(sidebar)
        action_section.pack(fill="x", padx=15, pady=(0, 15))

        self.compress_btn = ctk.CTkButton(action_section, text="🗜️ Compress Image",
                                          command=self.compress_image_gui,
                                          height=40, fg_color="#800080", hover_color="#5a005a")
        self.compress_btn.pack(fill="x", padx=10, pady=5)

        self.decompress_btn = ctk.CTkButton(action_section, text="🔄 Decompress Image",
                                            command=self.decompress_image_gui,
                                            height=40, fg_color="#800080",hover_color="#5a005a")
        self.decompress_btn.pack(fill="x", padx=10, pady=5)

        results_section = ctk.CTkFrame(sidebar)
        results_section.pack(fill="x", padx=15, pady=(0, 15))

        ctk.CTkLabel(results_section, text="Results",
                     font=("Arial", 14, "bold")).pack(pady=(10, 10))

        self.cr_label = ctk.CTkLabel(results_section,
                                     text="Compression Ratio: --\nBits per Pixel: --",
                                     font=("Arial", 12),
                                     justify="left")
        self.cr_label.pack(pady=(0, 10))

        header = ctk.CTkFrame(main_content, height=60)
        header.grid(row=0, column=0, sticky="ew", padx=20, pady=20)
        header.grid_propagate(False)

        ctk.CTkLabel(header, text="Image Preview",
                     font=("Arial", 18, "bold")).pack(expand=True)

        images_container = ctk.CTkFrame(main_content)
        images_container.grid(row=1, column=0, sticky="nsew", padx=20, pady=(0, 20))
        images_container.grid_columnconfigure((0, 1, 2), weight=1, uniform="cols")
        images_container.grid_rowconfigure(1, weight=1)

        orig_panel = ctk.CTkFrame(images_container)
        orig_panel.grid(row=0, column=0, sticky="nsew", padx=10, pady=10)

        ctk.CTkLabel(orig_panel, text="Original Image",
                     font=("Arial", 14, "bold")).pack(pady=(15, 10))

        self.orig_canvas = ctk.CTkLabel(orig_panel, text="No image loaded\n\nClick 'Load Image' to start",
                                        width=350, height=350,
                                        font=("Arial", 12),
                                        text_color="gray",
                                        corner_radius=10)
        self.orig_canvas.pack(padx=20, pady=20)

        comp_panel = ctk.CTkFrame(images_container)
        comp_panel.grid(row=0, column=1, sticky="nsew", padx=10, pady=10)

        ctk.CTkLabel(comp_panel, text="Compressed Preview",
                     font=("Arial", 14, "bold")).pack(pady=(15, 10))

        self.comp_canvas = ctk.CTkLabel(comp_panel, text="Not compressed\n\nCompress an image to see preview",
                                        width=350, height=350,
                                        font=("Arial", 12),
                                        text_color="gray",
                                        corner_radius=10)
        self.comp_canvas.pack(padx=20, pady=20)

        decomp_panel = ctk.CTkFrame(images_container)
        decomp_panel.grid(row=0, column=2, sticky="nsew", padx=10, pady=10)

        ctk.CTkLabel(decomp_panel, text="Decompressed Image",
                     font=("Arial", 14, "bold")).pack(pady=(15, 10))

        self.decomp_canvas = ctk.CTkLabel(decomp_panel, text="Not decompressed\n\nDecompress to see result",
                                          width=350, height=350,
                                          font=("Arial", 12),
                                          text_color="gray",
                                          corner_radius=10)
        self.decomp_canvas.pack(padx=20, pady=20)

    def display_image(self, img_array, canvas):
        try:
            img_uint8 = np.clip(img_array, 0, 255).astype(np.uint8)
            im = Image.fromarray(img_uint8)

            canvas_width = 350
            canvas_height = 350
            img_ratio = im.width / im.height
            canvas_ratio = canvas_width / canvas_height

            if img_ratio > canvas_ratio:
                new_width = canvas_width
                new_height = int(canvas_width / img_ratio)
            else:
                new_height = canvas_height
                new_width = int(canvas_height * img_ratio)

            im = im.resize((new_width, new_height), Image.Resampling.LANCZOS)
            imgtk = ImageTk.PhotoImage(im)
            canvas.configure(image=imgtk, text="")
            canvas.image = imgtk
        except Exception as e:
            canvas.configure(image=None, text=f"Display Error\n{str(e)}")

    def load_image_gui(self):
        file_path = filedialog.askopenfilename(
            filetypes=[("Image Files", "*.png;*.jpg;*.jpeg;*.bmp;*.tiff")]
        )
        if file_path:
            try:
                self.original_image = load_image(file_path)
                self.image_path = file_path
                self.display_image(self.original_image, self.orig_canvas)
            except Exception as e:
                messagebox.showerror("Error", f"Failed to load image: {str(e)}")

    def compress_image_gui(self):
        if self.original_image is None:
            messagebox.showwarning("Warning", "Please load an image first")
            return

        try:
            block_size = int(self.block_size.get())
            k = int(self.k_size.get())

            if block_size < 2:
                messagebox.showwarning("Warning", "Block size must be at least 2")
                return
            if k < 2:
                messagebox.showwarning("Warning", "Codebook size must be at least 2")
                return

            blocks, h_blocks, w_blocks, padded_img, pad_h, pad_w = split_into_blocks(self.original_image, block_size)

            if len(blocks) < k:
                messagebox.showwarning("Warning",
                                       f"Codebook size K ({k}) is larger than number of blocks ({len(blocks)}). Using {len(blocks)} instead.")
                k = len(blocks)

            codebook, labels = kmeans(blocks, k)

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

            cr, bpp = compute_compression_ratio(self.original_image, self.codebook_file, self.labels_file,
                                                self.metadata_file)
            self.cr_label.configure(text=f"Compression Ratio: {cr:.2f}:1\nBits per Pixel: {bpp:.4f}")

        except Exception as e:
            messagebox.showerror("Error", f"Compression failed: {str(e)}")

    def decompress_image_gui(self):
        if not hasattr(self, 'codebook_file') or not all(
                os.path.exists(f) for f in [self.codebook_file, self.labels_file, self.metadata_file]):
            messagebox.showwarning("Warning", "Please compress an image first")
            return

        try:
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

        except Exception as e:
            messagebox.showerror("Error", f"Decompression failed: {str(e)}")

if __name__ == "__main__":
    app = VQApp()
    app.mainloop()
