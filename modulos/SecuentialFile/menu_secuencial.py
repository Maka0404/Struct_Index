import tkinter as tk
from tkinter import messagebox, simpledialog
import ctypes

# Cargar la biblioteca compartida
mi_codigo = ctypes.CDLL('./secuentialfile.so')

# Definición de las funciones C++
mi_codigo.initIndice.argtypes = [ctypes.c_char_p]
mi_codigo.agregarRegistroDesdePython.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
mi_codigo.eliminarRegistroDesdePython.argtypes = [ctypes.c_char_p, ctypes.c_int]
mi_codigo.buscarRegistroDesdePython.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
mi_codigo.buscarPorRangoDesdePython.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.POINTER(ctypes.c_char_p)), ctypes.POINTER(ctypes.c_int)]
mi_codigo.imprimirIndiceDesdePython.restype = None
mi_codigo.liberar.restype = None

# Inicializar el índice
mi_codigo.initIndice(b'estudiantes10.csv')

# Funciones de la interfaz
def agregar_registro():
    nuevo_registro = simpledialog.askstring("Agregar Registro", "Ingresa el nuevo registro (Nombre,Apellido,Carrera,Edad,Codigo):")
    if nuevo_registro:
        mi_codigo.agregarRegistroDesdePython(b'estudiantes10.csv', nuevo_registro.encode('utf-8'))
        messagebox.showinfo("Éxito", "Registro agregado con éxito.")
        actualizar_pantalla()

def eliminar_registro():
    edad = simpledialog.askinteger("Eliminar Registro", "Ingresa la edad del registro a eliminar:")
    if edad is not None:
        mi_codigo.eliminarRegistroDesdePython(b'estudiantes10.csv', edad)
        messagebox.showinfo("Éxito", f"Registro con edad {edad} eliminado.")
        actualizar_pantalla()

def buscar_registro():
    edad = simpledialog.askinteger("Buscar Registro", "Ingresa la edad del registro a buscar:")
    if edad is not None:
        resultado = ctypes.c_char_p()
        mi_codigo.buscarRegistroDesdePython(edad, ctypes.byref(resultado))
        messagebox.showinfo("Resultado", resultado.value.decode('utf-8'))

def buscar_por_rango():
    edad_min = simpledialog.askinteger("Buscar por Rango", "Ingresa la edad mínima:")
    edad_max = simpledialog.askinteger("Buscar por Rango", "Ingresa la edad máxima:")
    if edad_min is not None and edad_max is not None:
        resultados = ctypes.POINTER(ctypes.c_char_p)()
        size = ctypes.c_int()
        mi_codigo.buscarPorRangoDesdePython(edad_min, edad_max, ctypes.byref(resultados), ctypes.byref(size))
        mensaje = "Resultados:\n"
        for i in range(size.value):
            mensaje += f"- {resultados[i].decode('utf-8')}\n"
        messagebox.showinfo("Resultados", mensaje)

def actualizar_pantalla():
    # Aquí puedes implementar la lógica para actualizar la pantalla con el índice
    output.delete('1.0', tk.END)  # Limpiar el área de texto
    mi_codigo.imprimirIndiceDesdePython()  # Imprimir el índice en la consola
    output.insert(tk.END, "Índice impreso en la consola.")  # Actualizar la interfaz

# Crear la ventana principal
ventana = tk.Tk()
ventana.title("Gestión de Estudiantes")

# Menú
menu = tk.Menu(ventana)
ventana.config(menu=menu)

# Submenú de Operaciones
operaciones_menu = tk.Menu(menu)
menu.add_cascade(label="Operaciones", menu=operaciones_menu)
operaciones_menu.add_command(label="Agregar Registro", command=agregar_registro)
operaciones_menu.add_command(label="Eliminar Registro", command=eliminar_registro)
operaciones_menu.add_command(label="Buscar Registro", command=buscar_registro)
operaciones_menu.add_command(label="Buscar por Rango", command=buscar_por_rango)

# Área de texto para mostrar resultados (opcional)
output = tk.Text(ventana, height=10, width=50)
output.pack(pady=10)

# Cerrar la biblioteca al finalizar
def on_closing():
    mi_codigo.liberar()
    ventana.destroy()

ventana.protocol("WM_DELETE_WINDOW", on_closing)

# Iniciar el bucle principal de la interfaz
ventana.mainloop()
