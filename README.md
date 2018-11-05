# Space_Intersection
feature points detection, feature points matching, error feature points elimination(RANSAC), and matching points coordinates calculation of 2 images
使用时只需更改图片路径，解析好的XML excel路径（excel）放到文件夹，输出结果路径


输出文件格式：
[img1_x,  img1_y] 		 [img2_x, img2_y]  			  X  			  Y  		  Z   		  descriptor1  		   descriptor2 
[778.888916, 1843.333374] [764.444336, 3327.777832] 560913.319950 4320591.974042 304.406170航眪9D呩v-?鯹?U並?獋F$6?

img_x & img_y 为影像坐标 
X & Y & Z     为影像同名点地理坐标 
descriptor	  为影像坐标的描述算子 每个算子32像素 每个像素1字节 每次可以以char的形式读入
