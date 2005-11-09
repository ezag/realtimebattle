/**************************************************
 joypad_robot_analog.c -- Copyright(c) 2005 Clarence Risher <sparr0@gmail.com>
 based on
 joypad_robot.c -- Copyright(c) 1999 Jiro Sekiba <jir@hello.to>
 This program is destributed under the GPL.
 
 This robot takes input from /dev/input/js0 and expects an analog joystick
 with at least 2 axes and 2 buttons (shoot and brake)
 
 The defines and logic included here assume that axis 0 is right/left
 and axis 1 is down/up, with a range of -32767 to +32767
 **************************************************/
#include<stdio.h>
#include<fcntl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

#define __USE_GNU		/* for M_PIl etc */
#include<math.h>

#include<linux/joystick.h>

//assume that Messagetypes.h is here.  If not, try <Messagetypes.h>
#include <Messagetypes.h>

//modify this define to adjust the 'dead spot' at the center of your joystick
#define DEADAREA 1500

//modify this define to set the maximum range of your joystick, -JOYRANGE to +JOYRANGE will be used
//using a value slightly lower than the actual maximum will put an anti-dead-area around the outer edge of the joystick area
#define JOYRANGE 30000

double GAME_OPT[13];
double rotate_angle = M_PIl / 36;	/* 5 deg */
int joy_fd = -1;


void
initialize (int num)
{
  if (num == 1)
    {
      puts ("Name Joybot_Analog");
      puts ("Colour C2ED42 42EDC2");

      fflush (stdout);
    }
}

void
gameoption (int type, double value)
{
  GAME_OPT[type] = value;
}

void
debug (char *msg)
{
  printf ("Debug %s\n", msg);
}

void
turn (float speed)
{
  if (speed > DEADAREA || speed < -DEADAREA)	// joystick right or left
    printf ("Rotate 1 %f\n", GAME_OPT[ROBOT_MAX_ROTATE] * speed / -JOYRANGE);
  else				// joystick centered (within dead area)
    printf ("Rotate 1 0.0\n");
}

void
shoot (void)
{
  printf ("Shoot %f\n", GAME_OPT[SHOT_MAX_ENERGY]);
}

void
accel (float speed)
{
  if (speed > DEADAREA)		// joystick down
    printf ("Accelerate %f\n",
	    GAME_OPT[ROBOT_MAX_ACCELERATION] * speed / -JOYRANGE);
  else if (speed < -DEADAREA)	// joystick up
    printf ("Accelerate %f\n",
	    GAME_OPT[ROBOT_MIN_ACCELERATION] * speed / JOYRANGE);
  else				// joystick centered (within dead area)
    printf ("Accelerate 0.0\n");
}

void
brake_on (void)
{
  puts ("Brake 1.0");
}

void
brake_off (void)
{
  puts ("Brake 0.0");
}

void
idle (void)
{
  /* do something for idle time */
}

void
main_loop (void)
{
  struct timeval tv;
  fd_set rfds;
  int retval;
  struct js_event event;

  char buf[2048];
  int type;
  double value;
  int num;

  /* Wait up to one seconds. */
  tv.tv_sec = 1;
  tv.tv_usec = 0;

  while (1)
    {
      FD_ZERO (&rfds);
      FD_SET (0, &rfds);
      FD_SET (joy_fd, &rfds);
      retval = select (6, &rfds, NULL, NULL, &tv);

      if (FD_ISSET (joy_fd, &rfds))
	{
	  read (joy_fd, &event, sizeof (struct js_event));
	  switch (event.type)
	    {

	    case JS_EVENT_BUTTON:
	      switch (event.number)
		{
		case 0:	/* brake */
		  if (event.value)	/* pressed */
		    brake_on ();
		  else		/* released */
		    brake_off ();
		  break;
		case 1:
		  if (event.value)	/* pressed */
		    shoot ();
		  break;
		case 2:	/* shoot */
		  /* do something if your joypad has button 3 */
		  break;
		case 3:
		  /* do something if your joypad has button 4 */
		  break;
		default:
		  break;	/* do nothing */
		}
	      break;
	    case JS_EVENT_AXIS:
	      switch (event.number)
		{
		case 0:	/* right/left */
		  turn (event.value);
		  break;
		case 1:	/* down/up */
		  accel (event.value);
		  break;
		default:
		  /* do nothing */
		  break;
		}
	      break;
	    default:
	      break;
	    }
	}
      else if (FD_ISSET (0, &rfds))	/* message from RTB */
	{
	  fgets (buf, 2048, stdin);
	  if (strncmp (buf, "Initialize", 10) == 0)
	    {
	      sscanf (buf, "Initialize %i", &num);
	      initialize (num);
	    }
	  else if (strncmp (buf, "GameOption", 10) == 0)
	    {
	      sscanf (buf, "GameOption %i %lf", &type, &value);
	      gameoption (type, value);

	    }
	  else if (strncmp (buf, "Dead", 4) == 0)
	    {
	      break;
	    }
	}
      else
	{
	  idle ();
	}
      fflush (stdout);
    }
}

int
open_joystick (char *name)
{
  joy_fd = open (name, O_RDONLY);
  if (joy_fd < 0)
    {
      fprintf (stderr, "Can't open file %s\n", name);
      return (1);
    }
  return (0);
}

int
main (void)
{
  if (open_joystick ("/dev/js0"))
    if (open_joystick ("/dev/input/js0"))
      {
	printf ("RobotOption %i 1\n", USE_NON_BLOCKING);
	puts ("Name Joybot_Analog");
	puts ("Colour C2ED42 42EDC2");
	fflush (stdout);
	fprintf (stderr, "Exiting joystick bot ...\n");
	sleep (3);
	puts ("Print Could not open the joystick device, exiting ...");
	return -1;
      }
  printf ("RobotOption %i 1\n", USE_NON_BLOCKING);
  fflush (stdout);
  main_loop ();
}
