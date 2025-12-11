import numpy as np
from PIL import Image
import math

def fast_power(base, exp):
    result = 1
    while exp > 0:
        if exp & 1:
            result *= base
        base *= base
        exp >>= 1
    return result

def predictor_quantization(image_path, b):
    # Open image and convert to grayscale
    img = Image.open(image_path).convert('L')  # 'L' = 8-bit grayscale
    arr = np.array(img, dtype=int)
    h, w = arr.shape
    
    # Initialize predictor array
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
    minE = np.min(diff[1:, 1:]) if h > 1 and w > 1 else 0
    maxE = np.max(diff[1:, 1:]) if h > 1 and w > 1 else 0
    
    target = fast_power(2, b)
    target = max(target, 1)
    
    range_val = maxE - minE
    step = 1.0 if range_val == 0 else range_val / target
    
    # Build quantization map
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
    
    # Quantize and dequantize
    q_diff = np.zeros_like(diff)
    deq_diff = np.zeros_like(diff)
    
    for i in range(1, h):
        for j in range(1, w):
            normalized = (diff[i, j] - minE) / step
            num = int(normalized + 1e-9)
            num = max(0, min(num, target - 1))
            q_diff[i, j] = num
            
            L, R = mp[num]
            deq_diff[i, j] = (L + R) // 2
    
    # Reconstruct image
    orig = np.zeros_like(arr)
    orig[0, :] = arr[0, :]
    orig[:, 0] = arr[:, 0]
    orig[1:, 1:] = pred[1:, 1:] + deq_diff[1:, 1:]
    
    # Save reconstructed image
    reconstructed_img = Image.fromarray(np.clip(orig, 0, 255).astype(np.uint8))
    reconstructed_img.save("reconstructed.png")
    
    return reconstructed_img, q_diff, deq_diff, mp

# Example usage
image_path = "bw_image.png"  # Replace with your black & white image path
b = 2  # Number of quantization bits
reconstructed_img, q_diff, deq_diff, mp = predictor_quantization(image_path, b)
reconstructed_img.show()
