#produces RGB565 colors out of RGB888
input_file_name = 'sand_colors.txt'

def rgb_hex565(red, green, blue):
  return ((int(red / 255 * 31) << 11) | (int(green / 255 * 63) << 5) | (int(blue / 255 * 31)))

with open(input_file_name, 'r') as color_list_file:
  for color in color_list_file.readlines():
    #color in hex as RRGGBB
    r = int(color[0:2], 16)
    g = int(color[2:4], 16)
    b = int(color[4:6], 16)
    print(f'{hex(rgb_hex565(r,g,b))},', end= '')
    
    
