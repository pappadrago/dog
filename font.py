from PIL import Image

def process_sprite_sheet(input_path, output_path):
    # 1. Carica l'immagine sorgente e convertila in RGB gestendo la trasparenza (alpha channel)
    img_src = Image.open(input_path)
    
    # Se l'immagine ha trasparenza (RGBA o palette con trasparenza), la incolliamo su uno sfondo nero o bianco
    if img_src.mode in ('RGBA', 'LA') or (img_src.mode == 'P' and 'transparency' in img_src.info):
        # Crea uno sfondo nero (o bianco se preferisci)
        background = Image.new("RGB", img_src.size, (0, 0, 0))
        if img_src.mode == 'P':
            img_src = img_src.convert('RGBA')
        background.paste(img_src, mask=img_src.split()[3]) # Usa il canale alpha come maschera
        img_src = background
    else:
        img_src = img_src.convert("RGB")

    width, height = img_src.size
    
    cell_src_size = 6
    cell_dst_size = 8
    
    # Verifica le dimensioni
    if width % cell_src_size != 0 or height % cell_src_size != 0:
        print("Attenzione: le dimensioni dell'immagine non sono multiple di 6. Verranno tagliati i pixel in eccesso.")
        
    cols = width // cell_src_size
    rows = height // cell_src_size
    total_cells = cols * rows
    
    # 2. Crea l'immagine di destinazione (colonna verticale di celle 8x8)
    dst_width = cell_dst_size
    dst_height = total_cells * cell_dst_size
    
    img_dst = Image.new("RGB", (dst_width, dst_height), color=(0, 0, 0))
    
    # 3. Estrazione delle celle 6x6 e inserimento nelle celle 8x8 in verticale
    current_dst_y = 0
    
    for r in range(rows):
        for c in range(cols):
            # Ritaglia la cella sorgente 6x6
            box_src = (
                c * cell_src_size,
                r * cell_src_size,
                (c + 1) * cell_src_size,
                (r + 1) * cell_src_size
            )
            cell_img = img_src.crop(box_src)
            
            # Crea una cella di destinazione 8x8 nera
            cell_dst = Image.new("RGB", (cell_dst_size, cell_dst_size), color=(0, 0, 0))
            
            # Incolla la cella 6x6 dentro la 8x8 (in alto a sinistra, oppure usa (1,1) se vuoi centrarla di 1 pixel)
            cell_dst.paste(cell_img, (0, 0))
            
            # Posiziona la cella 8x8 nell'immagine verticale finale
            img_dst.paste(cell_dst, (0, current_dst_y))
            
            current_dst_y += cell_dst_size

    # 4. Salva il file risultante
    img_dst.save(output_path)
    print(f"Fatto! Immagine salvata in: {output_path}")
    print(f"Totale celle elaborate: {total_cells} disposte in colonna.")

# Esempio di utilizzo:
# process_sprite_sheet("input_sprites.png", "output_verticale.png")
process_sprite_sheet("round_6x6.png", "font.bmp")