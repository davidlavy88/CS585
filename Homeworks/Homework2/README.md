<p style="font-size: 120%;"><b>Problem definition</b></p>

<p>Track an object in a video. This will be performed by using a picture of the isolated object and use it as a template. Then using a webcam the user will hold the object and the program will draw a bounding box where the template matching algorithm finds the object.
<ul>
  <li>Challenges</li>
  <ul>
    <li>Scale variance: When the object is too far or too close the template matching will not work correctly as it will try to find a small pen in a big box.</li>
    <li>Rotation invariance: Template will have only one direction. If it is asymmetric, rotating the object in the video will cause errors in the matching</li>
    <li>Good video quality and illumination: These two factors can add a lot of noise to the camera. This happened to me using a Linux machine. We made sure we had mostly daylight and also good working cameras.</li>
    <li>Understand what's happening under the hood: As OpenCV uses a 'blackbox' to calculate the matrix of coeff. using the matchingtemplate method, we corroborated our results by doing an approximation of the values and make sure these were normalized in the range of [0, 1]</li>
  </ul>
  <li>Assumptions</li>
  <ul>
    <li>For this part, we assume that there was no rotation invariance, nor scale invariance.</li>
    <li>As we're using Normalized Cross-Correlation, lightning and exposure aren't a great deal. So as long as the template and the camera video are visible, the tracking works just fine.</li>
    <li>The object to track in this case is a rectangle.</li>
  </ul>
 </ul>

<hr>

<p style="font-size: 120%;"><b>Method and Implementation</b></p>

<p>The method we used here is simple. A call of the matchTemplate function in OpenCV. To do this first we stream our webcam (or a video previously recorded) together with a template we will like to use. In our experiment we took a picture holding a card and then crop it. This will be our template for tracking. Then we call our function MatchingMethod and show the rectangle where our template is located. The function is described below.

<p>For this task we created one function called MatchingMethod, what this does is performing the template matching using match_method=CV_TM_SQDIFF_NORMED which is the one discussed in class. Once this was calculated we looked in our image where was the highest coeffcient located and draw a rectangle in it.

<hr>

<p style="font-size: 120%;"><b>Experiments</b></p>
<p>For this first part we took the image of a Charlie card. We did this procedure offline and online. As this part was relatively simple the tracking was done just once. But is important to highlight that we tried this setting in 3 different locations at 3 rooms illuminated differently.</p>

<p>Metrics
 <ul>
   <li>Detection rate: This was about 90%. There were certain times in the matching that the rectangle went off the true matching. This was mostly caused by rapid movements and also rotation of the card which made it hard to track.</li>
   <li>Running time: For the calculation of detection described above we used a video of length of 15 seconds.</li>   
 </ul>

<hr>

<p style="font-size: 120%;"><b>Results</b></p>   

<table style="width:100%">
 <caption><b>Results</b></caption>
 <tr>
   <th><b>Method</b></th>
   <th><b>Original Image</b></th>
   <th><b>Result Image</b></th>
 </tr>
 <tr>
   <td><b>Matching Method using NCC (Online)</b></td>
   <td><a href="http://imgur.com/Jchk1b1"><img src="http://imgur.com/Jchk1b1.png" width="480" height="360" title="Offline image" /></a></td>
   <td><a href="http://imgur.com/BIlrGEU"><img src="http://imgur.com/BIlrGEU.png" width="480" height="360" title="Template Matching Offline" /></a>
 </tr>
 <tr>
   <td><b>Matching Method using NCC (Offline)</b></td>
   <td><a href=""><img src="http://imgur.com/QfHSVaR.png" width="480" height="360" title="Online Image (webcam)" /></a></td>
   <td><a href="http://imgur.com/LeAEvfZ"><img src="http://imgur.com/LeAEvfZ.png" width="480" height="360" title="Template Matching Online" /></a>
 </tr>
</table>

<hr>

<p style="font-size: 120%;"><b>Discussion</b></p>

<p>
 <ul>
   <li>An advantage of our method is the use of NCC which makes our system invariant to lightning and exposure.</li>
   <li>The weakness is that we aren't accounting for scale and rotation invariance, which makes a different angle for our template make our system 50% of the time find another matching rectangle.</li>
   <li>We can improve our method by using a set of downsampled images to account for the scale invariance. Also for rotation we can find edges of our stream and verify the direction of potential matchings and rotate them in the same position as our template</li>
 </ul>
</p>
   
<hr>
 
<p style="font-size: 120%;"><b>Conclusions</b></p>
<p>This was a good introduction to template-based matching algorithms. Future experiments will involve try different algorithms and verify which one is more efficient.</p>

<hr>

<p style="font-size: 120%;"><b>Bibliography</b></p>
<p> - http://docs.opencv.org/doc/tutorials/imgproc/histograms/template_matching/template_matching.html

<hr>

<p style="font-size: 120%;"><b>Part 2</b></p>
<p style="font-size: 120%;"><b>Problem definition</b></p>

   <p>Track an our hand in the video. Also detect 3 shapes and output which is the shape we are making and delineate the shape of the hand. 
   <p>We are addressing 3 different challenges here. 
   <p>First we need to perform a skin detection algorithm to isolate only the skin in our video. Then we will do a template matching with this output together with our template, which has been processed with the skin detection as well so we only have skin as a grayscale image and the background will be black. After this we will do 3 different template matching and look at their coefficients. The highest of these 3 coefficients will tell us which shape is the one we are making. Finally, we will copy the code we did in part one to create a rectangle in the matching and using our skin detection image we will delineate the shape and put this contour back in the original stream.
<ul>
 <li>Challenges</li>
 <ul>
   <li>Scale variance: Same as in our part 1, scale invariance is a problem we need to address, however we found out that the hand detection will work well in a certain range and it also detects the shape.</li>
   <li>Rotation invariance: This is a very difficult problem, that we could not address, shapes of the hand are more difficult to find orientation. We read some papers about how to do this that will be mentioned at the end.</li>
   <li>Background and noise supression: When there is a 'lot of skin' in the background this impacted our system with low performance.</li>
   <li>Rapid movement of the hand. Usually in our system we have our face and our hands. If the hands move rapidly we will lose track of them, and we found out that the system goes for the face as a potential match. This was improved if we moved our hand slowly.</li>
 </ul>
 <li>Assumptions</li>
 <ul>
   <li>We assumed that our skin detection varies according to lightning and exposure, this is because when using the same parameters in different environments, there are big blobs in different images. To address this we created a calibration system that will give us the parameters we need for skin detection.</li>
   <li>Also we tried to mantain a uniform background so it is easier to isolate the sking with our algorithm</li>
   <li>The object to track in this case will be bounded in a rectangle.</li>
 </ul>
 
</ul>

<hr>

<p style="font-size: 120%;"><b>Method and Implementation</b></p>

<p>The method we used here is as we described above. We first take the HSV parameters for skin detection. Later we start streaming images from the webcam, transform these into HSV and apply our skin detection function.This will give us a grayscale image where the grayscale part will only be applied to our skin. We do the same for our templates. 
<p>Once this is done we apply our MatchingMethod described in part one, but with a little modification that will be decribed below. This function will give us the coefficient of the template matching which will be compared between the three of these matchings and finally the one that's the biggest will determine what shape is being used.
<p>Finally we will use the rectangle from our MatchingMethod function and extract the contour of this which will be our hand shape. 

<p>For this task we created a separate function called skinHSV which we can perform a manual skin detection calibration by determining the HSV min and max parameters that later will be used in our main program. 
<p>In our main program we have the function mySkinDetect that was done in class, but this was modified for our method using HSV parameters. And it will output a grayscale image.
<p>My MatchingMethod function has been modified by outputting the matching rectangle where we will draw our contour and this contour later will be drawn in the streaming image. Also it outputs the maximum coefficient of the matrix given by the matchTemplate OpenCV function.
<p>Finally a function called myMaxD will give us the maximum coefficient of the three we got from doing MatchingMethod.

<hr>

<p style="font-size: 120%;"><b>Experiments</b></p>
<p>For this second part we took many different templates, we tried our algorithm with three main shapes which are: Open Hand, Peace sign and Spock ('live long and prosper'). However it is important to point that we used other templates as well (Right point, left point, up point, fist, etc). Then we carried our experiments in many different scenarios, with people in it, dark background, light background. Also we performed many different offline and online experiments. Specially in my case, my light bulbs at home are not really powerful and this created a very dark image that was really difficult to perform my skin detection. </p>

<p>Metrics
 <ul>
   <li>Detection rate: This was about 67%. The tracking of the first two shapes worked pretty good, but the third one we had problems because the matching was not done properly. After debugging our algorithm we could not find what was the cause of this problem.</li>
   <li>Running time: This running time was more extense and we used videos of length about 45 - 60 seconds trying different shapes over and over again.</li>
 </ul>   

<hr>

<p style="font-size: 120%;"><b>Results</b></p>   

<table style="width:100%">
  <caption><b>Table of templates</b></caption>
 <tr>
   <th><b>Template 1</b></th>
   <th><b>Template 2</b></th>
   <th><b>Template 3</b></th>
 </tr>
 <tr>
   <td><a href="http://i.imgur.com/92E2jjN"><img src="http://i.imgur.com/92E2jjN.png" width="240" height="320" title="Template1" /></a></td>
   <td><a href="http://i.imgur.com/pOaXgPT"><img src="http://i.imgur.com/pOaXgPT.png" width="240" height="320" title="Template2" /></a></td>
   <td><a href="http://i.imgur.com/Ouq50Ji"><img src="http://i.imgur.com/Ouq50Ji.png" width="240" height="320" title="Template3" /></a>
 </tr>
 <tr>
   <td><a href="http://i.imgur.com/S9Z1MS5"><img src="http://i.imgur.com/S9Z1MS5.png" width="480" height="360" title="Matching template 1" /></a></td>
   <td><a href="http://i.imgur.com/xyHFAXy"><img src="http://i.imgur.com/xyHFAXy.png" width="480" height="360" title="Matching template 2" /></a></td>
   <td><a href="http://i.imgur.com/u2bjDJi"><img src="http://i.imgur.com/u2bjDJi.png" width="480" height="360" title="Matching Template 3" /></a>
 </tr>
</table>

<hr>

<p style="font-size: 120%;"><b>Discussion</b></p>

<p>
 <ul>
   <li>The strength of our project is how we addressed our template matching. We  think that doing a previous skin detection isolates other features that are irrelevant and this will make the matching easier.</li>
   <li>Another strength is our program that calculates the HSV parameters in each situation, this is helpful because we can try in almost any situation, and we can be sure that skin detection will be optimized.
   <li>The weakness is that are not accounting for scale and rotation invariance. A slight move of for example our open hand makes it seem like it is a peace sign, this is confusing.</li>
   <li>We can improve our method by using a set of downsampled images to account for the scale invariance. For rotation invariance we can use 3D cameras that will also capture depth, this will give us extra information and the shapes will not be mixed as we saw what happened in our algorithm.</li>
 </ul>
</p>

<p>Confussion Matrix (Done for the first 3 seconds of each shape):
<p>Done to a total of 270 frames:
<p>
<style type="text/css">
.tg  {border-collapse:collapse;border-spacing:0;}
.tg td{font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;}
.tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;}
.tg .tg-oopb{color:#cb0000}
.tg .tg-wvtg{background-color:#34bdc9}
.tg .tg-jhgd{background-color:#3186af}
.tg .tg-rpj7{background-color:#ad6864}
.tg .tg-698h{background-color:#000000}
</style>
<table class="tg">
<tr>
  <th class="tg-031e">Results\Data</th>
  <th class="tg-031e">Open Hand</th>
  <th class="tg-031e">Peace Sign</th>
  <th class="tg-031e">Spock</th>
  <th class="tg-rpj7">Overall Classifications</th>
  <th class="tg-wvtg">Precision</th>
</tr>
<tr>
  <td class="tg-031e">Open Hand</td>
  <td class="tg-oopb">85</td>
  <td class="tg-oopb">4</td>
  <td class="tg-oopb">1</td>
  <td class="tg-rpj7">21</td>
  <td class="tg-wvtg">94%</td>
</tr>
<tr>
  <td class="tg-031e">Peace Sign</td>
  <td class="tg-oopb">6</td>
  <td class="tg-oopb">80</td>
  <td class="tg-oopb">4</td>
  <td class="tg-rpj7">90</td>
  <td class="tg-wvtg">89%</td>
</tr>
<tr>
  <td class="tg-031e">Spock</td>
  <td class="tg-oopb">18</td>
  <td class="tg-oopb">63</td>
  <td class="tg-oopb">9</td>
  <td class="tg-rpj7">90</td>
  <td class="tg-wvtg">10%</td>
</tr>
<tr>
  <td class="tg-jhgd">Recall</td>
  <td class="tg-jhgd">78%</td>
  <td class="tg-jhgd">54%</td>
  <td class="tg-jhgd">64%</td>
  <td class="tg-698h" colspan="2"></td>
</tr>
</table>
 
<hr>
 
<p style="font-size: 120%;"><b>Conclusions</b></p>
<p>This was a very challenging project. Many different methods were tried, a lot of errors were made, but overall it was good learning in how we can do shape detection. I hope to improve this version and make it work with the 3D points I mentioned above using a Kinect.</p>

<hr>

<p style="font-size: 120%;"><b>Bibliography</b></p>
<p> - http://docs.opencv.org/doc/tutorials/imgproc/histograms/template_matching/template_matching.html

<hr>
