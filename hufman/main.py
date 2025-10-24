class Node:
    def __init__(self, char=None, freq=0):
        self.char = char
        self.freq = freq
        self.left = None
        self.right = None


nodes = []


def calculate_frequencies(word):
    global nodes
    nodes = []
    frequencies = {}
    for char in word:
        if char not in frequencies:
            freq = word.count(char)
            frequencies[char] = freq
            nodes.append(Node(char, freq))


def build_huffman_tree():
    while len(nodes) > 1:
        nodes.sort(key=lambda x: x.freq)
        left = nodes.pop(0)
        right = nodes.pop(0)
        merged = Node(freq=left.freq + right.freq)
        merged.left = left
        merged.right = right
        nodes.append(merged)
    return nodes[0]


def generate_huffman_codes(node, current_code, codes):
    if node is None:
        return
    if node.char is not None:
        codes[node.char] = current_code
    generate_huffman_codes(node.left, current_code + '0', codes)
    generate_huffman_codes(node.right, current_code + '1', codes)


def huffman_encoding(word):
    calculate_frequencies(word)
    root = build_huffman_tree()
    codes = {}
    generate_huffman_codes(root, '', codes)
    return codes


def huffman_decoding_from_files(encoded_file="encoded.txt", codes_file="codes.txt", output_file="decoded.txt"):
    try:
        with open(encoded_file, "r", encoding="utf-8") as f:
            encoded_word = f.read().strip()
    except FileNotFoundError:
        print(f"File '{encoded_file}' not found!")
        return

    codes = {}
    try:
        with open(codes_file, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if line:
                    char, code = line.split(":")
                    codes[char] = code
    except FileNotFoundError:
        print(f"File '{codes_file}' not found!")
        return

    current_code = ''
    decoded_chars = []
    code_to_char = {v: k for k, v in codes.items()}

    for bit in encoded_word:
        current_code += bit
        if current_code in code_to_char:
            decoded_chars.append(code_to_char[current_code])
            current_code = ''

    decoded_message = ''.join(decoded_chars)

    with open(output_file, "w", encoding="utf-8") as f:
        f.write(decoded_message)

    print(f"Decoding complete! Result saved to '{output_file}'.")


# ==== MENU ====
while True:
    print("\nHuffman Menu:")
    print("1. Encode from input.txt")
    print("2. Decode from encoded.txt + codes.txt")
    print("3. Exit")
    choice = input("Choose an option (1-3): ")

    if choice == "1":
        try:
            with open("input.txt", "r", encoding="utf-8") as f:
                word = f.read().strip()
        except FileNotFoundError:
            print("File 'input.txt' not found!")
            continue

        if not word:
            print("File is empty!")
            continue

        codes = huffman_encoding(word)
        encoded_word = ''.join(codes[char] for char in word)

        with open("output.txt", "w", encoding="utf-8") as f:
            f.write(f"Original Word: {word}\n")
            f.write(f"Encoded Huffman Code: {encoded_word}\n\n")
            f.write("Codes Table:\n")
            for char, code in codes.items():
                f.write(f"{char}:{code}\n")

        # Save encoded only for decoding later
        with open("encoded.txt", "w", encoding="utf-8") as f:
            f.write(encoded_word)
        with open("codes.txt", "w", encoding="utf-8") as f:
            for char, code in codes.items():
                f.write(f"{char}:{code}\n")

        print("Encoding complete! Results saved to 'output.txt', 'encoded.txt', and 'codes.txt'.")

    elif choice == "2":
        huffman_decoding_from_files()
    elif choice == "3":
        print("Exiting...")
        break
    else:
        print("Invalid choice! Please choose 1, 2, or 3.")
