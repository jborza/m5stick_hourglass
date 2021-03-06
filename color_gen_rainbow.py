#using a list of standard web-safe colors as RGB hex strings
input_file_name = 'web_safe_colors.txt'

def rgb_hex565(red, green, blue):
  return ((int(red / 255 * 31) << 11) | (int(green / 255 * 63) << 5) | (int(blue / 255 * 31)))

with open(input_file_name, 'r') as color_list_file:
  for color in color_list_file.readlines():
    #using shorthand three-digit form, produce the original RRGGBB color
    r = int(color[0]+color[0], 16)
    g = int(color[1]+color[1], 16)
    b = int(color[2]+color[2], 16)
    print(f'{hex(rgb_hex565(r,g,b))},', end= '')
    
    
