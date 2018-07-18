import tensorflow as tf
import numpy as np
import os
import matplotlib.pyplot as plt
import matplotlib.cm as cm

path_model = os.path.join(os.path.dirname(__file__),"pymodule","model-export")
is_training = False
STAN_SIZE=[56,56]
ORIGINAL_SIZE=[68,68]

sess_img=tf.Session()
Graph_d=tf.Graph()


with Graph_d.as_default(): #7sec
  
    Sess_d = tf.Session()
    clear_devices = True
    saver = tf.train.import_meta_graph(path_model+ '.meta', clear_devices=clear_devices)
    saver.restore(Sess_d, path_model)
    img_pl = Graph_d.get_tensor_by_name("input_placeholder/img_place:0")
    pred_label= Graph_d.get_tensor_by_name("output_layer/final:0")
    is_training_pl = Graph_d.get_tensor_by_name("input_placeholder/Placeholder:0")



def convert_inputs(img):

    #img = np.array(img).astype("int32")
    if(img.shape[-1]>1):    #ok       
        img = np.expand_dims(img, -1)        

    img = np.reshape(img,(ORIGINAL_SIZE[0], ORIGINAL_SIZE[1], 1))   
    img = img / 255.0  #ok   
   
    img=tf.image.resize_bilinear(np.expand_dims(img,0),STAN_SIZE)   #ok
  
    img=sess_img.run(img)

    return img


def model_sumon(img):#0.1sec  
        
    np.clip(img,0,255,img)
    
    img=convert_inputs(img)    

    with Graph_d.as_default():

        feed_dict = {img_pl: img,is_training_pl: is_training}
        pred_lb= Sess_d.run(pred_label, feed_dict=feed_dict)
        np.squeeze(pred_lb)

    return int(pred_lb)

