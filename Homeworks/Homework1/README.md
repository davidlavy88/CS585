<h2>Problem definition</h2>
<p>Add your original face image and the processed image(s) and a short explanation about your processing algorithm.

<hr>

<p style="font-size: 120%;"><b>Experiments</b></p>
<p>Original Image
<p>This picture I took it on my first trip to Japan in Kyoto. One of the best places to visit.

<p>Experiment 1 - Grayscale Image
<p>I used the OpenCV function cvtColor with argument RGB2GRAY, this uses the algorithm to convert the pixel: x = 0.21r + 0.72g + 0.07b. If you need more information about this visit this <a
href="http://docs.opencv.org/doc/user_guide/ug_mat.html#primitive-operations">link</a>

<p>Experiment 2 - Canny Edge Detector
<p>Canny Edge Detector acts by smoothing the image using a Gaussian filter and then find the gradients of the image, and using a min and max threshold it determines potential edges. This is a very popular edge detector method. And I used the function Canny from OpenCV. A simple tutorial can be found <a href="http://docs.opencv.org/doc/tutorials/imgproc/imgtrans/canny_detector/canny_detector.html">here</a> 

<hr>

<h2>Method and Implementation</h2>
<p>Add your original face image and the processed image(s) and a short explanation about your processing algorithm.

<hr>

<h2>Results</h2>

<table style="width:100%">
 <caption><b>Results</b></caption>
 <tr>
   <th><b>Method</b></th>
   <th><b>Original Image</b></th>
   <th><b>Result Image</b></th>
 </tr>
 <tr>
   <td><b>Grayscale</b></td>
   <td><a href="http://imgur.com/1FQEfrS"><img src="http://i.imgur.com/1FQEfrS.png" width="480" height="360" title="Original Image" /></a></td>
   <td><a href="http://imgur.com/yRi4mzU"><img src="http://i.imgur.com/yRi4mzU.png" width="480" height="360" title="Grayscale Image" /></a>
 </tr>
 <tr>
   <td><b>Canny Edge Detector</b></td>
   <td><a href="http://imgur.com/1FQEfrS"><img src="http://i.imgur.com/1FQEfrS.png" width="480" height="360" title="Original Image" /></a></td>
   <td><a href="http://imgur.com/laFfmIO"><img src="http://i.imgur.com/laFfmIO.png" width="480" height="360" title="Canny Edge Image" /></a>
 </tr>
</table>

<hr>

<h2>Conclusions</h2>
<p>This first lab gave us an intro on how to use OpenCV, learn the syntax, remember some C++ stuff and get ready for lab 2 which will be more tedious.

<hr>

To compile the program using CMake, do the following:  </br>

$ cd <hw1_directory> </br>
$ cmake . </br>
$ make </br>

This will generate an executable which can be run by: </br>

$ ./playImages </br>
