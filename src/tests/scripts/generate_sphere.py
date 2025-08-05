import math
import os

def generate_sphere(vertices_count, filename):
    vertices = []
    faces = []

    # Рассчитываем количество параллелей и меридианов
    n_parallels = int(math.sqrt(vertices_count / 2))
    n_meridians = n_parallels * 2

    # Генерируем вершины
    for i in range(n_parallels + 1):
        theta = i * math.pi / n_parallels
        for j in range(n_meridians):
            phi = j * 2 * math.pi / n_meridians
            x = math.sin(theta) * math.cos(phi)
            y = math.sin(theta) * math.sin(phi)
            z = math.cos(theta)
            vertices.append((x, y, z))

    # Генерируем грани
    for i in range(n_parallels):
        for j in range(n_meridians):
            a = i * n_meridians + j + 1
            b = i * n_meridians + (j + 1) % n_meridians + 1
            c = (i + 1) * n_meridians + (j + 1) % n_meridians + 1
            d = (i + 1) * n_meridians + j + 1
            
            if i != 0:
                faces.append((a, b, d))
            if i != n_parallels - 1:
                faces.append((b, c, d))

    # Создаем директорию если ее нет
    os.makedirs(os.path.dirname(filename), exist_ok=True)

    # Записываем в файл
    with open(filename, 'w') as f:
        f.write("# Sphere with {} vertices\n".format(len(vertices)))
        for v in vertices:
            f.write("v {} {} {}\n".format(v[0], v[1], v[2]))
        for face in faces:
            f.write("f {} {} {}\n".format(*face))

# Генерируем сферы разного размера
sizes = [100, 1000, 10000, 100000, 1000000]
for size in sizes:
    filename = f"../../obj/sphere_{size}.obj"
    generate_sphere(size, filename)
    print(f"Generated {filename} with ~{size} vertices")
