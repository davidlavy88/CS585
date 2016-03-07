//opencv libraries
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//C++ standard libraries
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <ctype.h>
#include <iomanip>
#include <math.h>
#include <complex>
#include <numeric>
#include <cfloat>
#include <iterator>
#include <dirent.h>

using namespace cv;
using namespace std;

int main()
{
    string input;
    vector<string> all;
    ifstream infile;

    DIR *dir;
    struct dirent *ent;
    //MODIFY WITH CURRENT FOLDER
    if ((dir = opendir ("./CS585-AquariumImages")) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
        string num(ent->d_name);
//        if ((num[0] != ".") || (num != ".."))
        if (num != ".." )
        { if (num != ".")
            {
                string numero = num.substr(20,3);
                cout << "Picture number: " << numero << endl;
            }
        }
        else continue;
      }
      closedir (dir);
    } else {
      /* could not open directory */
      perror ("");
      return EXIT_FAILURE;
    }
}
