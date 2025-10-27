class Node:
    def __init__(self, name, left=None, right=None):
        self.name = name
        self.left = left
        self.right = right


def generate_codes(root, code, codes):
    if not root:
        return

    
    if not root.left and not root.right:
        codes[root.name] = code
        return

    generate_codes(root.left, code + "0", codes)
    generate_codes(root.right, code + "1", codes)



n = int(input("enter the number of distinct chars: "))

# Using list of tuples instead of multiset: (frequency, Node)
st = []
for _ in range(n):
    s= input()
    prob = input()
    prob = int(prob)
    leaf = Node(s)
    st.append((prob, leaf))

# Sort by frequency to simulate multiset order
st.sort(key=lambda x: x[0])

# Build Huffman tree
while len(st) > 1:
    num1 = st.pop(0)  # smallest
    num2 = st.pop(0)  # next smallest

    combined_freq = num1[0] + num2[0]
    parent = Node(num1[1].name + num2[1].name, num1[1], num2[1])

    # Insert back while keeping sorted order
    st.append((combined_freq, parent))
    st.sort(key=lambda x: x[0])

# Root of the tree
root = st[0][1]

# Generate Huffman codes
codes = {}
generate_codes(root, "", codes)

# Print the codes
for symbol, code in codes.items():
    print(f"{symbol} -> {code}")
