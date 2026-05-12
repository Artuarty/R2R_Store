# Layout de la Escena — R2R Store

## Dimensiones del espacio
Ancho: ~8m  |  Profundidad: ~6m  |  Alto: ~3m
Origen del mundo: esquina trasera izquierda del piso

## IMPORTANTE
Antes de codificar las posiciones, analiza las coordenadas
que te proporcionaré.

## Posición inicial de la cámara
Position: vec3(0.0f, 1.7f, 4.0f)   — entrada de la tienda
Yaw: -90.0f  |  Pitch: 0.0f

## Paleta de colores R2R
Primario:   #1A3A6B  — azul profundo
Secundario: #2E6DB4  — azul medio
Acento:     #4FC3F7  — azul eléctrico (neón)
Neutro:     #CFD8DC  — gris frío

## Luces de la escena
- 1 luz direccional: exterior, simula luz de día por ventanas
  direction: vec3(-0.5, -1.0, -0.3), color: vec3(1.0, 0.98, 0.95)
- 2-4 luces puntuales: lámparas del techo (posiciones según layout)
  color: vec3(1.0, 0.97, 0.88), intensidad animada (ver ANIM_08)
- Ambiente global: vec3(0.05, 0.07, 0.10), strength: 0.15

## Lista de modelos y sus coordenadas (.obj disponibles en /models/)
Nombre, Pos_X, Pos_Y, Pos_Z, Rot_X, Rot_Y, Rot_Z
REF_Pared_Refrigeradores, 5.8403, 0.9979, 1.8552, 0.0000, 0.0000, 0.0000
REF_Estructura_tienda_completa_R2R, -1.0423, -1.7782, 3.1096, 0.0000, 0.0000, 0.0000
REF_Piso_completo_r2r, -2.0385, -1.4435, 0.6247, 0.0000, 0.0000, 0.0000
REF_Rack_Refri_2, 6.0667, 3.0237, 1.7411, 0.0000, 0.0000, 0.0000
REF_Refri_Puerta_3, 5.7682, 2.6401, 1.7057, 0.0000, 0.0000, 0.0000
REF_Rack_Refri_6, 6.0667, -0.2595, 1.7914, 0.0000, 0.0000, 0.0000
REF_Refri_Puerta_4, 5.7682, 1.8717, 1.7057, 0.0000, 0.0000, 0.0000
REF_Ventana_1, -2.9264, -2.7337, 1.9687, 0.0000, 0.0000, 0.0000
REF_Ventana_2, 3.2589, -2.7337, 1.9687, 0.0000, 0.0000, 0.0000
REF_Rack_Bebidas, -3.8657, -0.9869, 1.9687, 0.0000, 0.0000, 0.0000
REF_Rack_Cigarros, -3.6416, -0.9115, 3.3790, 0.0000, 0.0000, 0.0000
REF_Trabe_Techo, 0.7707, 4.2845, 3.6220, 0.0000, 0.0000, 0.0000
REF_Puerta_princ_Izq, -1.6287, -2.7753, 1.8163, 0.0000, 0.0000, 0.0000
REF_Mostrador, -2.3605, -0.4460, 1.3043, 0.0000, 0.0000, 0.0000
REF_Estante_2, 2.3128, -0.7936, 1.2789, 0.0000, 0.0000, 0.0000
REF_Puerta_princ_Der, 0.3599, -2.7559, 1.8163, 0.0000, 0.0000, 0.0000
REF_Estante_1, 2.3128, 2.0625, 1.2789, 0.0000, 0.0000, 0.0000
REF_Lampara_2, 2.9418, 0.4617, 3.6875, 0.0000, 0.0000, 0.0000
REF_Lampara_1, -0.7389, 0.4617, 3.6875, 0.0000, 0.0000, 0.0000
REF_Folleto_R2R_8, 1.9487, -2.7494, 2.6805, 0.0000, 0.0000, 0.0000
REF_Folleto_R2R_7, 4.6855, -2.7494, 2.6805, 0.0000, 0.0000, 0.0000
REF_Folleto_R2R_9, -2.9448, -2.7494, 2.6805, 0.0000, 0.0000, 0.0000
REF_Folleto_R2R_6, -2.9448, -2.7330, 2.6805, 0.0000, 0.0000, 0.0000
REF_Folleto_R2R_5, 1.9487, -2.6838, 2.6805, 0.0000, 0.0000, 0.0000
REF_Folleto_R2R_4, 4.6855, -2.6838, 2.6805, 0.0000, 0.0000, 0.0000
REF_Folleto_R2R_3, 4.8314, 4.4046, 2.2949, 0.0000, 0.0000, 0.0000
REF_Folleto_R2R_1, -2.3008, 4.4046, 2.5134, 0.0000, 0.0000, 0.0000
REF_Folleto_R2R_2, 1.2052, 4.4046, 2.5134, 0.0000, 0.0000, 0.0000
REF_Cartel_Wanted, 0.5767, -2.6183, 2.6814, 0.0000, 0.0000, 0.0000
REF_Mueble_Cafetera2, -1.5421, 4.0310, 1.1479, 0.0000, 0.0000, 0.0000
REF_Varios_Cafetera2, -1.1199, 4.0250, 1.6010, 0.0000, 0.0000, 0.0000
REF_Taza_Cafetera2_2, -1.5280, 3.8866, 1.6441, 0.0000, 0.0000, 0.0000
REF_Taza_Cafetera2_1, -1.9085, 3.8420, 1.6428, 0.0000, 0.0000, 0.0000
REF_Cafetera2_Boquilla_3, -1.5284, 3.8920, 1.7731, 0.0000, 0.0000, 0.0000
REF_Cafetera2_Boquilla_2, -1.7165, 3.8846, 1.7731, 0.0000, 0.0000, 0.0000
REF_Cafetera2_Body, -1.7202, 3.9554, 1.8071, 0.0000, 0.0000, 0.0000
REF_Cafetera2_Boquilla_1, -1.9045, 3.8799, 1.7731, 0.0000, 0.0000, 0.0000
REF_Varios_Cafetera1, -2.1732, 4.0250, 1.6010, 0.0000, 0.0000, 0.0000
REF_Taza_Cafetera1_2, -2.5889, 3.8633, 1.6441, 0.0000, 0.0000, 0.0000
REF_Taza_Cafetera1_1, -2.7597, 3.8420, 1.6428, 0.0000, 0.0000, 0.0000
REF_Mueble_Cafetera1, -2.5954, 4.0310, 1.1479, 0.0000, 0.0000, 0.0000
REF_Cafetera1_Boquilla_1, -2.9729, 3.8800, 1.7718, 0.0000, 0.0000, 0.0000
REF_Cafetera1_Boquilla_2, -2.7692, 3.8859, 1.7682, 0.0000, 0.0000, 0.0000
REF_Cafetera1_Boquilla_3, -2.5817, 3.8926, 1.7688, 0.0000, 0.0000, 0.0000
REF_Cafetera1_Body, -2.7735, 3.9554, 1.8071, 0.0000, 0.0000, 0.0000
REF_Helados1_Body, -0.0463, 4.0744, 1.1728, 0.0000, 0.0000, 0.0000
REF_Helados1_Tapa, -0.0749, 4.0513, 1.5434, 0.0000, 0.0000, 0.0000
REF_Paletas_1, 0.3965, 3.8992, 1.5323, 0.0000, 0.0000, 0.0000
REF_Helados2_Tapa, 1.7075, 4.0986, 1.5811, 0.0000, 0.0000, 0.0000
REF_Paletas_2, 2.1576, 3.9429, 1.5928, 0.0000, 0.0000, 0.0000
REF_Helados2_Body, 1.7494, 4.0986, 1.1728, 0.0000, 0.0000, 0.0000
REF_Pared_Trasera, 6.7713, 0.9405, 3.6986, 3.1416, 0.0000, -0.0000
REF_Caja2_Body, -1.7447, -0.8409, 2.2193, 0.0000, 0.0000, 0.0000
REF_Caja1_Body, -3.5847, -0.0187, 2.2193, 0.0000, 0.0000, 0.0000
REF_Camara_Soporte, -3.9672, 3.7412, 3.5083, 0.0000, 0.0000, 0.0000
REF_Camara_Head, -3.8878, 3.6584, 3.5431, 0.0000, 0.0000, 0.0000
REF_Poste_R2R, -5.6911, -3.8938, 0.6702, 0.0000, 0.0000, 0.0000
REF_Basura, 6.0189, -3.5158, 1.0280, 0.0000, 0.0000, 0.0000
REF_Logo_Tienda_Entrada_Principal, -0.6976, -3.8182, 4.1515, 1.5661, -0.0000, 0.0000
REF_Logo_poste_R2R, -5.6493, -4.1155, 11.6522, 1.5661, -0.0000, 0.0000
REF_Refri_Puerta_1, 5.7682, 4.1769, 1.7057, 0.0000, 0.0000, 0.0000
REF_Rack_Refri_3, 6.0667, 2.2553, 1.7914, 0.0000, 0.0000, 0.0000
REF_Rack_Refri_4, 6.0667, 1.4869, 1.7914, 0.0000, 0.0000, 0.0000
REF_Rack_Refri_5, 6.0667, 0.5089, 1.7914, 0.0000, 0.0000, 0.0000
REF_Rack_Refri_7, 6.0667, -1.0278, 1.7914, 0.0000, 0.0000, 0.0000
REF_Rack_Refri_1, 6.0667, 3.7921, 1.7028, 0.0000, 0.0000, 0.0000
REF_Rack_Refri_8, 6.0667, -1.7962, 1.7914, 0.0000, 0.0000, 0.0000
REF_Refri_Puerta_6, 5.7682, -0.6442, 1.7057, 0.0000, 0.0000, 0.0000
REF_Refri_Puerta_5, 5.7682, 0.1242, 1.7057, 0.0000, 0.0000, 0.0000
REF_Refri_Puerta_7, 5.7682, -1.4126, 1.7057, 0.0000, 0.0000, 0.0000
REF_Refri_Puerta_2, 5.7682, 3.4085, 1.7057, 0.0000, 0.0000, 0.0000
REF_Refri_Puerta_8, 5.7682, -2.1810, 1.7057, 0.0000, 0.0000, 0.0000
REF_Chorro_Cafe_Cafetera2_Taza_2, -1.5279, 3.8887, 1.7703, 0.0000, 0.0000, 0.0000
REF_Taza_Cafe, -1.5279, 3.8887, 1.6889, 0.0000, 0.0000, 0.0000
REF_Caja2_Cajon, -2.0572, -0.9243, 1.9916, 0.0000, 0.0000, 0.0000
REF_Hielo_Puerta, -3.2104, 2.4565, 1.8957, 0.0000, 0.0000, 0.0000
REF_Hielo_Body, -3.7458, 2.1500, 1.6217, 0.0000, 0.0000, 0.0000
REF_Hielo_Bolsa, -3.4436, 2.0144, 1.8423, 0.0000, 0.0000, 0.0000
REF_Caja1_Cajon, -3.5052, -0.3321, 1.9916, 0.0000, 0.0000, 0.0000