<p style="font-size: 120%;"><b>Problem definition</b></p>

<p>
In previous assignments, we have demonstrated the problems and solutions to identifying and analysing objects, but an important aspect we did not cover was the tracking of those object. In this assignment, we attempted to use various methods to track objects in a video. Using the segmented video, we tracked bats based on their centroids.
</p>
<ul>
    <li>Challenges</li>
    <ul>
        <li>Implementation: Initially we implemented an alpha-beta filter to predict the bats movements, then used a greedy algorithm to assign the locations in the next frame to the tracked bats. This naive approach proved buggy, so we then implemented the more complicated Kalman Filter with the hungarian tracking method.</li>
        <li>Occlusion: Bats fly in front of each other, occlusing the image and temporarily sharing a centroid.</li>
        <li>Entering and exiting bats: Bats fly in and out of the frame, as well as flying in the z-axis so that they become segmented.</li>
        <li>Spurious detection: There are times where there are spurious objects detected by the segemntation algorithm that should not be tracked.</li>
    </ul>
    <!--
    <li>Assumptions</li>
    <ul>
        <li>Aquarium: We will make our system lightning invariant by only using our value channel from the HSV space, and consider small isolated motion pixels as fish, due to the fact that using and erode and dilation step gets rid of a lot of the fish in the images.</li>
        <li>Bats: We will be using the grayscale images and because our algorithm worked fairly well (e.g. no considerable noise) we assumed that all the blobs we got are bats.</li>
        <li>Cells: We made the assumption that when getting our contour of the cells, because of the bright values around them we get stripes that they don't connect, our assumption was that if these stripes are close enough they belong to the same boundary of the cell and therefore by using morphological operations we can get the whole contour and shape of the cell.</li>
    </ul>
    -->
</ul>

<hr>
<h2> Method and Implementation </h2>

<p>We initially implemented the alpha-beta filter to predict the location of the bats, and used a greedy algorthim to assign predictions to found centroids in the next frame. We were able to create a work-around for occlusion; if no centroids were found in a given radius, we allowed the tracked bat to use a centroid already taken previously by the greedy algorithm (because when bats are occluded, they temporarily share a centroid). To adjust for bats exiting the screen, if the projected x or y value was outside the size of the picture, we stopped tracking that centroid. For any centroids not previously tracked by bats, we created a new bat-tracking structure for it. This implementation had limited success, confined to regions where the bats were sufficiently far apart (the top).</p>
<p>The results of greedy/alpha-beta left much to be desired, so we started implementing a more sophisticated method: the Kalman Filter with Hungarian assignments. The Hungarian algorithm is designed to deal with too many/too few bats when the matrix isn't NxN; if there are too few bats, we start tracking the new centroids. If there are too many bats, we first consider occlusion; if the point reappears according to what the Kalman Filter predicts, we jump there. If this does not happen within 10 frames, we consider the bat lost and stop tracking it.
</p>


<hr>
<h2>Experiments</h2>
<p>
Our initial experiment involved tracking the bats with greedy/alpha-beta, followed by tracking the bats with Hungarian/Kalman. We assigned each bat a new random color, and drew the trajectory of the bat in that color.
</p>


<hr>
<h2> Results</h2>

<table style="width:100%">
    <caption><b>Results</b></caption>
    <tr>
        <th><b>Original</b></th>
        <th><b>Greedy Algorithm - Alpha-Beta Filter</b></th>
        <th><b>Hungarian Algorithm - Kalman Filter</b></th>
    </tr>
    <tr>
        <td><div align="center">
<a href="http://www.youtube.com/watch?feature=player_embedded&v=kvyncQ2jHPU
" target="_blank"><img src="http://img.youtube.com/vi/kvyncQ2jHPU/0.jpg" 
alt="Original Bats" width="240" height="180" border="10" /></a>
        </div></td>
        <td><div align="center">
<iframe width="420" height="315" src="https://www.youtube.com/embed/6lQ9wC0grC0" frameborder="0" allowfullscreen></iframe>
        </div></td>
        <td><div align="center">
<iframe width="420" height="315" src="https://www.youtube.com/embed/w9mQGjM5TmI" frameborder="0" allowfullscreen></iframe>
        </div></td>
    </tr>
   </table>

<p>
<hr>
<h2> Discussion </h2>

<p>
The greedy algorthim with an alpha-beta filter had some moderate success. Bats we tracked to a degree, especially in areas where there weren't interfering bats. The issues with this approach are self-evident, and due to the greedy/non-optimal nature of the algorithm. The greedy algorithm is especially in problematic in areas where bats are clustered because if the greedy algorithm takes the wrong centroid early-on, it causes a cascade of wrongly tracked centroids.
</p>
<p>
The Hungarian algorithm with a Kalman filter proved more successful, with much smoother and reliable tracks. It fails to re-track bats after occlusion in at least one case, but is very successful otherwise. One minor problem is the way we trace the bats has a bug where is a bat flies over the path it is erased, but that is not a problem with the algorithm.
</p>
<hr>
<h2> Conclusions </h2>

<p>
Our results demonstrate the pros and cons of various existing methods in object tracking. The greedy algorithm, though much easier to implement, was less effective at assigning the bats. The Hungarian algorithm, partnered with the Kalman filter produced strong results. Although we found it suffiecient to use the Kalman filter with only the past data point, an area for further research would be implementing multiple hypothesis tracking.
</p>


<hr>
<h2> Credits and Bibliography </h2>
<p>
Hungarian algorithm reference: https://github.com/Smorodov/Multitarget-tracker <a href="https://github.com/Smorodov/Multitarget-tracker">  </a>10/26/15
</p>
<p>
OpenCV Kalman filter: http://docs.opencv.org/master/dd/d6a/classcv_1_1KalmanFilter.html#gsc.tab=0 <a href="http://docs.opencv.org/master/dd/d6a/classcv_1_1KalmanFilter.html#gsc.tab=0">  </a>10/26/15
</p>
<p>
Segmented videos provided in the assignment
</p>
<p>
Partners: Sean Smith, Sean Matuszak
</p>
