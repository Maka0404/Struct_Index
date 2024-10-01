import csv
import random

# 50 Nombres aleatorios
nombres = ['Lucía', 'Marta', 'Paula', 'Marcos', 'Luisa', 'Manuela', 'Carlos', 'José', 'Pedro', 'Ana', 
           'Laura', 'Cristina', 'Juan', 'Sofía', 'Diego', 'Miguel', 'Jorge', 'Teresa', 'Andrea', 'Luis', 
           'Fernando', 'Roberto', 'Raúl', 'Isabel', 'María', 'David', 'Pablo', 'Carmen', 'Julia', 'Javier',
           'Sara', 'Iván', 'Claudia', 'Alberto', 'Daniel', 'Antonio', 'Victoria', 'Alba', 'Gabriela', 'Francisco', 
           'Patricia', 'Alicia', 'Enrique', 'Nuria', 'Rosa', 'Esteban', 'Ángel', 'Raquel', 'Rubén', 'Alejandro']

# 50 Apellidos aleatorios
apellidos = ['Santos', 'Sanz', 'Fernández', 'Martín', 'Gutiérrez', 'Pérez', 'López', 'García', 'Rodríguez', 'Hernández',
             'Ruiz', 'Jiménez', 'Díaz', 'Moreno', 'Álvarez', 'Muñoz', 'Romero', 'Alonso', 'Giménez', 'Serrano',
             'Blanco', 'Ortiz', 'Molina', 'Delgado', 'Castro', 'Silva', 'Reyes', 'Vargas', 'Campos', 'Mendoza',
             'Rojas', 'Flores', 'Medina', 'Cabrera', 'Acosta', 'Cruz', 'Iglesias', 'Vega', 'Salazar', 'Bautista',
             'Ponce', 'Carrillo', 'Esparza', 'Vidal', 'Cano', 'Benítez', 'Guerra', 'Bravo', 'Soto', 'Morales']

# 50 Carreras aleatorias
carreras = ['Ingeniería de Sistemas', 'Ingeniería Civil', 'Ingeniería Electrónica', 'Bioingeniería', 'Ciencias de la Computación', 
            'Ingeniería Ambiental', 'Ingeniería Industrial', 'Matemáticas Aplicadas', 'Química', 'Física',
            'Ingeniería Mecánica', 'Ingeniería de Materiales', 'Ingeniería de Energía', 'Ingeniería Eléctrica', 'Ingeniería Química',
            'Biotecnología', 'Economía', 'Administración de Empresas', 'Arquitectura', 'Diseño Gráfico', 
            'Marketing', 'Derecho', 'Psicología', 'Medicina', 'Enfermería',
            'Ingeniería Agroindustrial', 'Geología', 'Ingeniería Minera', 'Ingeniería Naval', 'Odontología',
            'Farmacia', 'Ingeniería de Alimentos', 'Veterinaria', 'Biología', 'Turismo', 
            'Contabilidad', 'Negocios Internacionales', 'Cine', 'Antropología', 'Arqueología',
            'Ingeniería de Software', 'Ingeniería en Telecomunicaciones', 'Sociología', 'Filosofía', 'Historia', 
            'Ingeniería Biomédica', 'Ciencias Políticas', 'Relaciones Internacionales', 'Educación', 'Periodismo']

# Función para generar un DNI aleatorio de 8 dígitos
def generar_dni():
    return ''.join([str(random.randint(0, 9)) for _ in range(8)])

# Función para generar datos de estudiantes
def generar_datos(n):
    with open('estudiantes.csv', mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['Nombres', 'Apellidos', 'Carrera', 'Edad', 'DNI'])  # Cabeceras
        
        for _ in range(n):
            nombre = random.choice(nombres)
            apellido = random.choice(apellidos)
            carrera = random.choice(carreras)
            edad = random.randint(18, 30)
            dni = generar_dni()
            writer.writerow([nombre, apellido, carrera, edad, dni])

# Genera 100 registros de ejemplo
generar_datos(10)
