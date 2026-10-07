import tensorflow as tf
import numpy as np

# Descargar/cargar MNIST
(train_images, train_labels), (test_images, test_labels) = tf.keras.datasets.mnist.load_data()

# Normalizar imágenes: [0, 255] -> [0, 1]
train_images = train_images.astype(np.float32) / 255.0
test_images = test_images.astype(np.float32) / 255.0

# Convertir etiquetas a float32
train_labels = train_labels.astype(np.float32)
test_labels = test_labels.astype(np.float32)

# Guardar como ficheros binarios
train_images.tofile("../dfs/train_images.mat")
train_labels.tofile("../dfs/train_labels.mat")
test_images.tofile("../dfs/test_images.mat")
test_labels.tofile("../dfs/test_labels.mat")

print(train_images.shape)
print(train_labels.shape)
print(test_images.shape)
print(test_labels.shape)