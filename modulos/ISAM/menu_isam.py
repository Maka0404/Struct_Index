import tkinter as tk
from tkinter import messagebox, simpledialog
import ctypes

# Cargar la biblioteca compartida
mi_codigo = ctypes.CDLL('./isam.so')

# Definición de las funciones C++
mi_codigo.crearISAM.restype = ctypes.c_void_p
mi_codigo.agregarRegistro.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
mi_codigo.eliminarRegistroPorEdad.argtypes = [ctypes.c_void_p, ctypes.c_int]
mi_codigo.mostrarISAM.argtypes = [ctypes.c_void_p]
mi_codigo.leerCSV.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
mi_codigo.liberarISAM.argtypes = [ctypes.c_void_p]

# Crear la instancia del ISAM
isam = mi_codigo.crearISAM()

# Funciones de la interfaz
def cargar_datos():
    mi_codigo.leerCSV(isam, b'estudiantes10.csv')
    messagebox.showinfo("Cargar Datos", "Datos cargados desde el archivo CSV.")
    actualizar_pantalla()

def agregar_registro():
    nuevo_registro = simpledialog.askstring("Agregar Registro", "Ingresa el nuevo registro (Nombre, Apellido, Carrera, Edad, DNI):")
    if nuevo_registro:
        try:
            nombre, apellidos, carrera, edad, dni = nuevo_registro.split(',')
            mi_codigo.agregarRegistro(isam, nombre.encode('utf-8'), apellidos.encode('utf-8'), carrera.encode('utf-8'), int(edad), int(dni))
            messagebox.showinfo("Éxito", "Registro agregado exitosamente.")
            actualizar_pantalla()
        except ValueError:
            messagebox.showerror("Error", "Formato incorrecto. Usa: Nombre, Apellido, Carrera, Edad, DNI")

def eliminar_registro():
    edad = simpledialog.askinteger("Eliminar Registro", "Ingresa la edad del registro a eliminar:")
    if edad is not None:
        mi_codigo.eliminarRegistroPorEdad(isam, edad)
        messagebox.showinfo("Éxito", f"Registro con edad {edad} eliminado.")
        actualizar_pantalla()

def actualizar_pantalla():
    # Limpiar el área de texto
    output.delete('1.0', tk.END)
    
    # Mostrar el ISAM en la consola
    mi_codigo.mostrarISAM(isam)

    # Aquí podrías implementar una lógica para mostrar el ISAM en el área de texto si tienes esa funcionalidad

# Crear la ventana principal
ventana = tk.Tk()
ventana.title("Gestión de Estudiantes")

# Menú
menu = tk.Menu(ventana)
ventana.config(menu=menu)

# Submenú de Operaciones
operaciones_menu = tk.Menu(menu)
menu.add_cascade(label="Operaciones", menu=operaciones_menu)
operaciones_menu.add_command(label="Cargar Datos", command=cargar_datos)
operaciones_menu.add_command(label="Agregar Registro", command=agregar_registro)
operaciones_menu.add_command(label="Eliminar Registro", command=eliminar_registro)

# Área de texto para mostrar resultados
output = tk.Text(ventana, height=20, width=50)
output.pack()

# Ejecutar la aplicación
ventana.mainloop()

# Liberar el ISAM al cerrar
mi_codigo.liberarISAM(isam)
