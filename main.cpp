#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    cout << " Usage: display_image ImageToLoadAndDisplay" << endl;
    return -1;
  }

  Mat image;
  image = imread(argv[1], CV_LOAD_IMAGE_COLOR);

  if (!image.data) {
    cout << "Could not open or find the image" << std::endl;
    return -1;
  }

  struct termios tty;
  struct termios tty_old;
  char           buf = 'x';
  int            n   = 0;
  memset(&tty, 0, sizeof tty);

  int USB = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
  /* Error Handling */
  if (tcgetattr(USB, &tty) != 0) {
    cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
  }

  /* Save old tty parameters */
  tty_old = tty;

  /* Set Baud Rate */
  cfsetospeed(&tty, (speed_t)B115200);
  cfsetispeed(&tty, (speed_t)B115200);

  /* Setting other Port Stuff */
  tty.c_cflag &= ~PARENB;  // Make 8n1
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;

  tty.c_cflag &= ~CRTSCTS;        // no flow control
  tty.c_cc[VMIN]  = 1;            // read doesn't block
  tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
  tty.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines

  /* Make raw */
  cfmakeraw(&tty);

  /* Flush Port, then applies attributes */
  tcflush(USB, TCIFLUSH);
  if (tcsetattr(USB, TCSANOW, &tty) != 0) {
    std::cout << "Error " << errno << " from tcsetattr" << std::endl;
  }

  char outbuf[image.cols + 1];
  outbuf[image.cols + 1] = 's';
  namedWindow("Display window", WINDOW_NORMAL);
  for (int y = 0; y < image.rows; y++) {
    for (int x = 0; x < image.cols; x++) {
      Vec3b color = image.at<Vec3b>(Point(x, y));
      if (color[1] == 255) {
        color[0] = 0;
        color[2] = 0;
        image.at<Vec3b>(Point(x, y)) = color;
        outbuf[x] = 0;
      }else{
        outbuf[x] = 1;
      }
    }
    imshow("Display window", image);
    waitKey(100);
    write(USB, &outbuf, (image.cols+2));
    while (buf != 's') {
      n = read(USB, &buf, 1);
      cout << buf;
    }
    cout << endl;
    buf = 'x';
  }


  imshow("Display window", image);


  waitKey(0);
  return 0;
}
