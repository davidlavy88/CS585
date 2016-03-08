<p style="font-size: 120%;"><b>Problem definition</b></p>  

<p>Detecting and analysing objects is an important part of computer vision. We set out to detect certain objects in videos, detect connected components, lower the number of components, and perform statistics on these components. We did this for 3 sets of videos: an aquarium, cells, and bats.</p>
<ul>
    <li>Challenges</li>
    <ul>
        <li>Aquarium: This set of images was the most challenging one. This is because there exists multiple objects moving (big fish, small fish, seaweed). Also the reflection of the light and the pumped oxigen in the top right corner is deceiving to our algorithm, as we're trying to track the fish, but also the moving seaweed gets tracked. Another thing to notice is that small movement of the fish, and sometimes only just a part of their body moving as well as occlusion  makes the tracking difficult.</li>
        <li>Bats: The bats images weren't too difficult, except that we needed to come up with a good tracking algorithm for the lower part of the image where the brightness is too much that it gets mixed with the grayscale colors of the bats. Our first approach using iterative threshold wasn't as effective as the others as it is shown in our results.</li>
        <li>Cells: The cells images showed some difficult due to the existence of bright pixels around each cell, while having darker image regions. By using a classic threshold approach would either only get boundaries (that aren't continuous) or mixed the inside of the cell with the whole background.</li>
        <li>In terms of code implementation, challenges arised when creating our video, as this was hard in a Windows machine, but fairly easy in a Linux machine. Another challenge was the implementation of our connected components code, due to time constrains we weren't able to develop an efficient piece of code for this (although our algorithm ran fairly well and fast).</li>
    </ul>
    <li>Assumptions</li>
    <ul>
        <li>Aquarium: We will make our system lightning invariant by only using our value channel from the HSV space, and consider small isolated motion pixels as fish, due to the fact that using and erode and dilation step gets rid of a lot of the fish in the images.</li>
        <li>Bats: We will be using the grayscale images and because our algorithm worked fairly well (e.g. no considerable noise) we assumed that all the blobs we got are bats.</li>
        <li>Cells: We made the assumption that when getting our contour of the cells, because of the bright values around them we get stripes that they don't connect, our assumption was that if these stripes are close enough they belong to the same boundary of the cell and therefore by using morphological operations we can get the whole contour and shape of the cell.</li>
    </ul>
 
</ul>
    
    <hr>
    
    <h2> Method and Implementation </h2>

<p>For all data sets, we used two segmentation methods: iterative threshold and adaptive threshold. We implemented our own connected components algorithm quasi-recursively using a stack.  After all this we tailored each program to the dataset. For the aquarium, we didn't change the output (see discussion for explanation). For the bat dataset, we eroded to reduce single-pixel objects. For cells, we ran into an interesting problem; cells were being broken into many small components. To fix this, we dialted several times to fuse these components into one object, then eroded to approximate the cell.
For all data sets, statistics were generated for each object in the image and saved to a .csv file at runtime using opencv functions.
</p>
    
    
    <hr>
    <h2>Experiments</h2>
    <p>
    This problem was difficult to run experiments with. Most of our decisions were subjectively determined within the group as to what best approximated our given video.
    </p>
    
    
    <hr>
    <h2> Results</h2>

    <table style="width:100%">
      <caption><b>Table of templates</b></caption>
     <tr>
       <th><b>Original</b></th>
       <th><b>Adaptive Threshold</b></th>
       <th><b>Iterative Threshold</b></th>
     </tr>
     <tr>
       <td><div align="center">
<a href="http://www.youtube.com/watch?feature=player_embedded&v=dMMPcqAKGGw
" target="_blank"><img src="http://img.youtube.com/vi/dMMPcqAKGGw/0.jpg" 
alt="Aquarium Original" width="240" height="180" border="10" /></a>
 </div></td>
       <td><div align="center">
<a href="http://www.youtube.com/watch?feature=player_embedded&v=3mJvyBN0b5k
" target="_blank"><img src="http://img.youtube.com/vi/3mJvyBN0b5k/0.jpg" 
alt="Aquarium Adaptive Threshold" width="240" height="180" border="10" /></a>
 </div></td>
       <td><div align="center">
<a href="http://www.youtube.com/watch?feature=player_embedded&v=ZAAEebXXsN8
" target="_blank"><img src="http://img.youtube.com/vi/ZAAEebXXsN8/0.jpg" 
alt="Aquarium Iterative Threshold" width="240" height="180" border="10" /></a>
 </div></td>
     </tr>
     <tr>
       <td><div align="center">
<a href="http://www.youtube.com/watch?feature=player_embedded&v=n9pcr9bG8cs
" target="_blank"><img src="http://img.youtube.com/vi/n9pcr9bG8cs/0.jpg" 
alt="Bats Original" width="240" height="180" border="10" /></a>
 </div></td>
       <td><div align="center">
<a href="http://www.youtube.com/watch?feature=player_embedded&v=AwiPaXH-kNw
" target="_blank"><img src="http://img.youtube.com/vi/AwiPaXH-kNw/0.jpg" 
alt="Bats Adaptive Threshold" width="240" height="180" border="10" /></a>
 </div></td>
       <td><div align="center">
<a href="http://www.youtube.com/watch?feature=player_embedded&v=K3jvCzhRlKU
" target="_blank"><img src="http://img.youtube.com/vi/K3jvCzhRlKU/0.jpg" 
alt="Bats Iterative Threshold" width="240" height="180" border="10" /></a>
 </div></a></td>
     </tr>
     <tr>
       <td><div align="center">
<a href="http://www.youtube.com/watch?feature=player_embedded&v=Cfyuqb2gTIQ
" target="_blank"><img src="http://img.youtube.com/vi/Cfyuqb2gTIQ/0.jpg" 
alt="Cells Original" width="240" height="180" border="10" /></a>
 </div></td>
       <td><div align="center">
<a href="http://www.youtube.com/watch?feature=player_embedded&v=-R-VF0qM3n4
" target="_blank"><img src="http://img.youtube.com/vi/-R-VF0qM3n4/0.jpg" 
alt="Cells Adaptive Threshold" width="240" height="180" border="10" /></a>
 </div></td>
       <td><div align="center">
<a href="http://www.youtube.com/watch?feature=player_embedded&v=bdpSZiMov4Q
" target="_blank"><img src="http://img.youtube.com/vi/bdpSZiMov4Q/0.jpg" 
alt="Cells Iterative Threshold" width="240" height="180" border="10" /></a>
 </div></a></td>
     </tr>
   </table>

   <p>
   <table style="width:100%">
      <caption><b>Table of statistics for Adaptive Threshold</b></caption>
     <tr>
       <th><b>Aquarium</b></th>
       <th><b>Bats</b></th>
       <th><b>Cells</b></th>
     </tr>
     <tr>
       <th><table border="1">
<thead><tr><th title="Field #1">Frame Number</th>
<th title="Field #2">Area</th>
<th title="Field #3">Orientation (rad)</th>
<th title="Field #4">Circularity</th>
<th title="Field #5">Perimeter</th>
<th title="Field #6">Compactness</th>
</tr></thead>
<tbody><tr><td>1</td>
<td align="right">3.0</td>
<td align="right">0.1793850</td>
<td>0.490579</td>
<td align="right">6</td>
<td>1.33333</td>
</tr>
<tr><td>1</td>
<td align="right">4.5</td>
<td align="right">-0.0634864</td>
<td>0.0268898</td>
<td align="right">5</td>
<td>1.55556</td>
</tr>
<tr><td>1</td>
<td align="right">2.0</td>
<td align="right">0.6435010</td>
<td>0.242424</td>
<td align="right">6</td>
<td>2</td>
</tr>
<tr><td>1</td>
<td align="right">0.5</td>
<td align="right">2.3561900</td>
<td>0.333333</td>
<td align="right">5</td>
<td>14</td>
</tr>
<tr><td>1</td>
<td align="right">1.0</td>
<td align="right">0.0000000</td>
<td>1</td>
<td align="right">4</td>
<td>6</td>
</tr>
<tr><td>1</td>
<td align="right">4.0</td>
<td align="right">1.9756900</td>
<td>0.246753</td>
<td align="right">7</td>
<td>1.25</td>
</tr>
<tr><td>1</td>
<td align="right">0.5</td>
<td align="right">2.3561900</td>
<td>0.333333</td>
<td align="right">3</td>
<td>2</td>
</tr>
<tr><td>1</td>
<td align="right">17.0</td>
<td align="right">-0.0449429</td>
<td>0.107555</td>
<td align="right">13</td>
<td>0.882353</td>
</tr>
<tr><td>1</td>
<td align="right">6.0</td>
<td align="right">0.0000000</td>
<td>0.333333</td>
<td align="right">6</td>
<td>0.666667</td>
</tr>
<tr><td>1</td>
<td align="right">4.0</td>
<td align="right">-0.2513310</td>
<td>0.10785</td>
<td align="right">8</td>
<td>2.5</td>
</tr>
<tr><td>1</td>
<td align="right">10.5</td>
<td align="right">-0.1289640</td>
<td>0.370486</td>
<td align="right">11</td>
<td>0.857143</td>
</tr>
<tr><td>1</td>
<td align="right">0.0</td>
<td align="right">0.0000000</td>
<td>0</td>
<td align="right">2</td>
<td> -nan</td>
</tr>
<tr><td>1</td>
<td align="right">0.0</td>
<td align="right">0.0000000</td>
<td>0</td>
<td align="right">2</td>
<td> -nan</td>
</tr>
<tr><td>1</td>
<td align="right">2.0</td>
<td align="right">0.6172840</td>
<td>0.00901812</td>
<td align="right">11</td>
<td>4.5</td>
</tr>
<tr><td>1</td>
<td align="right">8.0</td>
<td align="right">0.3505460</td>
<td>0.130445</td>
<td align="right">7</td>
<td>0.625</td>
</tr>
<tr><td>1</td>
<td align="right">4.0</td>
<td align="right">0.0000000</td>
<td>0.0625</td>
<td align="right">4</td>
<td>1.5</td>
</tr>
<tr><td>1</td>
<td align="right">0.0</td>
<td align="right">0.0000000</td>
<td> inf</td>
<td align="right">1</td>
<td> inf</td>
</tr>
<tr><td>1</td>
<td align="right">15.0</td>
<td align="right">0.9407350</td>
<td>0.324684</td>
<td align="right">13</td>
<td>1</td>
</tr>
</tbody></table></th>
       <th><table border="1">
<thead><tr><th title="Field #1">Frame Number</th>
<th title="Field #2">Area</th>
<th title="Field #3">Orientation (rad)</th>
<th title="Field #4">Circularity</th>
<th title="Field #5">Perimeter</th>
<th title="Field #6">Compactness</th>
</tr></thead>
<tbody><tr><td>1</td>
<td align="right">2.0</td>
<td align="right">1.570800</td>
<td> 0.25</td>
<td align="right">4</td>
<td> 3</td>
</tr>
<tr><td>1</td>
<td align="right">4.5</td>
<td align="right">-0.270210</td>
<td> 0.489206</td>
<td align="right">6</td>
<td> 0.888889</td>
</tr>
<tr><td>1</td>
<td align="right">2.0</td>
<td align="right">0.000000</td>
<td> 0.25</td>
<td align="right">4</td>
<td> 3</td>
</tr>
<tr><td>1</td>
<td align="right">1.0</td>
<td align="right">1.570800</td>
<td> 0.333333</td>
<td align="right">3</td>
<td> 1</td>
</tr>
<tr><td>1</td>
<td align="right">8.0</td>
<td align="right">1.994430</td>
<td> 0.211487</td>
<td align="right">10</td>
<td> 1</td>
</tr>
<tr><td>1</td>
<td align="right">7.0</td>
<td align="right">1.216480</td>
<td> 0.304323</td>
<td align="right">9</td>
<td> 1.57143</td>
</tr>
<tr><td>1</td>
<td align="right">11.0</td>
<td align="right">1.773240</td>
<td> 0.281097</td>
<td align="right">9</td>
<td> 1</td>
</tr>
<tr><td>1</td>
<td align="right">26.0</td>
<td align="right">1.144410</td>
<td> 0.313441</td>
<td align="right">9</td>
<td> 0.423077</td>
</tr>
<tr><td>1</td>
<td align="right">6.0</td>
<td align="right">-0.231824</td>
<td> 0.0878283</td>
<td align="right">8</td>
<td> 1.66667</td>
</tr>
<tr><td>1</td>
<td align="right">9.5</td>
<td align="right">0.154026</td>
<td> 0.591698</td>
<td align="right">7</td>
<td> 0.526316</td>
</tr>
<tr><td>1</td>
<td align="right">23.5</td>
<td align="right">-0.480466</td>
<td> 0.510342</td>
<td align="right">8</td>
<td> 0.425532</td>
</tr>
<tr><td>1</td>
<td align="right">5.0</td>
<td align="right">0.314398</td>
<td> 0.330001</td>
<td align="right">6</td>
<td> 0.8</td>
</tr>
<tr><td>1</td>
<td align="right">2.5</td>
<td align="right">0.515188</td>
<td> 0.489206</td>
<td align="right">5</td>
<td> 2.8</td>
</tr>
<tr><td>1</td>
<td align="right">80.5</td>
<td align="right">1.750830</td>
<td> 0.139071</td>
<td align="right">37</td>
<td> 0.484472</td>
</tr>
<tr><td>1</td>
<td align="right">0.0</td>
<td align="right">0.000000</td>
<td> inf</td>
<td align="right">1</td>
<td> inf</td>
</tr>
<tr><td>1</td>
<td align="right">0.0</td>
<td align="right">0.000000</td>
<td> 0</td>
<td align="right">2</td>
<td> -nan</td>
</tr>
<tr><td>1</td>
<td align="right">7.0</td>
<td align="right">-0.548255</td>
<td> 0.0764445</td>
<td align="right">6</td>
<td> 0.571429</td>
</tr>
<tr><td>1</td>
<td align="right">32.5</td>
<td align="right">1.837870</td>
<td> 0.174926</td>
<td align="right">14</td>
<td> 0.369231</td>
</tr>
</tbody></table></th>
       <th><table border="1">
<thead><tr><th title="Field #1">Frame Number</th>
<th title="Field #2">Area</th>
<th title="Field #3">Orientation (rad)</th>
<th title="Field #4">Circularity</th>
<th title="Field #5">Perimeter</th>
<th title="Field #6">Compactness</th>
</tr></thead>
<tbody><tr><td align="right">1</td>
<td align="right">1504.5</td>
<td align="right">0.6032960</td>
<td align="right">0.4788380</td>
<td align="right">35</td>
<td align="right">0.0219342</td>
</tr>
<tr><td align="right">1</td>
<td align="right">6155.5</td>
<td align="right">1.7842100</td>
<td align="right">0.0237071</td>
<td align="right">223</td>
<td align="right">0.0359029</td>
</tr>
<tr><td align="right">1</td>
<td align="right">48.5</td>
<td align="right">0.7853980</td>
<td align="right">0.8620690</td>
<td align="right">5</td>
<td align="right">0.1443300</td>
</tr>
<tr><td align="right">1</td>
<td align="right">2568.0</td>
<td align="right">0.6149250</td>
<td align="right">0.2233270</td>
<td align="right">85</td>
<td align="right">0.0338785</td>
</tr>
<tr><td align="right">1</td>
<td align="right">1007.5</td>
<td align="right">0.5212410</td>
<td align="right">0.1131830</td>
<td align="right">63</td>
<td align="right">0.0605459</td>
</tr>
<tr><td align="right">1</td>
<td align="right">190.5</td>
<td align="right">-0.1139780</td>
<td align="right">0.2945030</td>
<td align="right">11</td>
<td align="right">0.0472441</td>
</tr>
<tr><td align="right">1</td>
<td align="right">1867.5</td>
<td align="right">0.0317736</td>
<td align="right">0.3489530</td>
<td align="right">54</td>
<td align="right">0.0278447</td>
</tr>
<tr><td align="right">1</td>
<td align="right">2143.5</td>
<td align="right">1.1892200</td>
<td align="right">0.2528400</td>
<td align="right">46</td>
<td align="right">0.0205272</td>
</tr>
<tr><td align="right">1</td>
<td align="right">2054.5</td>
<td align="right">1.4270800</td>
<td align="right">0.7261220</td>
<td align="right">60</td>
<td align="right">0.0301777</td>
</tr>
<tr><td align="right">2</td>
<td align="right">1114.5</td>
<td align="right">1.8805900</td>
<td align="right">0.6426750</td>
<td align="right">32</td>
<td align="right">0.0305070</td>
</tr>
<tr><td align="right">2</td>
<td align="right">2209.5</td>
<td align="right">1.7678900</td>
<td align="right">0.1507810</td>
<td align="right">92</td>
<td align="right">0.0425436</td>
</tr>
<tr><td align="right">2</td>
<td align="right">1446.0</td>
<td align="right">2.2378800</td>
<td align="right">0.8417280</td>
<td align="right">46</td>
<td align="right">0.0304288</td>
</tr>
<tr><td align="right">2</td>
<td align="right">2121.5</td>
<td align="right">0.6120910</td>
<td align="right">0.3741100</td>
<td align="right">69</td>
<td align="right">0.0334669</td>
</tr>
<tr><td align="right">2</td>
<td align="right">584.5</td>
<td align="right">1.4759100</td>
<td align="right">0.2002900</td>
<td align="right">42</td>
<td align="right">0.0684346</td>
</tr>
<tr><td align="right">2</td>
<td align="right">1733.0</td>
<td align="right">0.2365360</td>
<td align="right">0.3589690</td>
<td align="right">52</td>
<td align="right">0.0311598</td>
</tr>
<tr><td align="right">2</td>
<td align="right">240.5</td>
<td align="right">1.6591200</td>
<td align="right">0.0459334</td>
<td align="right">10</td>
<td align="right">0.0332640</td>
</tr>
<tr><td align="right">2</td>
<td align="right">1105.5</td>
<td align="right">1.0045600</td>
<td align="right">0.6039940</td>
<td align="right">34</td>
<td align="right">0.0289462</td>
</tr>
<tr><td align="right">2</td>
<td align="right">2529.5</td>
<td align="right">-0.0170933</td>
<td align="right">0.1816260</td>
<td align="right">60</td>
<td align="right">0.0245108</td>
</tr>
</tbody></table></th>
     </tr>
   </table>

        <hr>
    <h2> Discussion </h2>
    
    <p>
    Our methods proved very successful for Bats and Cells. The aquarium data set proved challenging due to the diversity of objects to be identified, the small size of objects, and many moving objects in the background. It was interesting that segmentation algorithm coices are project-dependant.
    </p>
    <p>
An interesting observation is that we created a "rainbow" effect when labelling components. Although we tried to conserve color, if the algorithm detected a different object first, they would switch color. For data sets with a large number of components or fast-moving objects, colors would appear to be strobing.
    </p>
    <p>
    A significant problem we encountered occurred with the aquarium data set. The results contained many small fish and lots of noise. We tried to reduce the noise by eroding then dilating, but this left us with boxes instead of fish. Fish would be eroded down to a single pixel, and the squares that resulted from dilating did not approximate the fish well. The small size of the fish and the sheer number of fish proved a problem.
    </p>
    <p>
    Another problem came from using iterative threshold on the cell dataset. There was not much color variation, so the iterative threshold did not give us a good result.
    </p>
    <hr>
    <h2> Conclusions </h2>
    
    <p>
    We had strong results, leading to the conclusion that segmentation methods of iterative and adaptive thresholding are highly useful, with the caveat that iterative thresholding should not be used on data sets that are close-to-uniformly-colored. The weaknesses were that these methods aren't necessarily robust; for each dataset, we needed to tailor a solution to "fix" the data. Eroding and dilating were very useful, except in the aquarium dataset. An improvement would be to find a way to lower the number of fish in the aquarium dataset.
    </p>
    
    
    <hr>
    <h2> Credits and Bibliography </h2>
  <!--
  <p>
    
    OpenCV template matching: <a href="http://docs.opencv.org/doc/tutorials/imgproc/histograms/template_matching/template_matching.html">http://docs.opencv.org/doc/tutorials/imgproc/his
        tograms/template_matching/template_matching.html</a> 9/23/15
     
    </p>
   -->
  <p>
  OpenCV statistics: http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html?highlight=findcontours#findcontours <a href="http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html?highlight=findcontours#findcontours">  </a>10/7/15
  </p>
  <p>
  Segmentation methods and connected components algorithm taught in-class by Margrit Betke
  </p>
    <p>
    Partners: Sean Smith, Sean Matuszak
    </p>
