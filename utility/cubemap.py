from PIL import Image

Image.MAX_IMAGE_PIXELS = 933120000
def extract_cubemap_sides(image_path):
    img = Image.open(image_path)
    width, height = img.size
    
    print(f"Image dimensions: {width}x{height}")
    
    # Check if image is of correct size
    if width % 4 != 0 or height % 3 != 0:
        print("Image dimensions are not compatible with cubemap.")
        return
    
    side_length = width // 4
    vertical_side_length = height // 3
    
    print(f"Side length: {side_length}, Vertical side length: {vertical_side_length}")

    sides = {
        'positive_x': (side_length * 2, vertical_side_length, side_length * 3, vertical_side_length * 2),
        'negative_x': (0, vertical_side_length, side_length, vertical_side_length * 2),
        'positive_y': (side_length, 0, side_length * 2, vertical_side_length),
        'negative_y': (side_length, vertical_side_length * 2, side_length * 2, vertical_side_length * 3),
        'positive_z': (side_length * 3, vertical_side_length, side_length * 4, vertical_side_length * 2),
        'negative_z': (side_length, vertical_side_length, side_length * 2, vertical_side_length * 2)
    }

    for side, coords in sides.items():
        print(f"Processing {side} side, coordinates: {coords}")
        side_img = img.crop(coords)
        side_img.save(f"{side}.png")
        print(f"{side}.jpg saved successfully.")
       
image_path = r"C:\Users\ymzym\Downloads\Standard-Cube-Map\StandardCubeMap.png"
extract_cubemap_sides(image_path)
