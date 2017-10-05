# Overview

Self Driving Car Engineer Nanodegree project. 
PID controller driving in a simulator.

## PID Controller

Proportionnal coefficient (P)
THe role of P is to make the car "drifting" toward the reference trajectory and reduce CTE (cross track error). It multiplies the CTE by P value.

Integral coefficient (I)
The role of the I term is to compensate systematic bias. For example if the steering of the car has been wrongly fixed and has a tendancy to steer a bit to the right, the I term will compensate that. It will accumulate the value of the surface between the position of the car and the reference trajectory (CTE) over time, and multiply this by the value of the I term.

Differential coefficient (D)
Once the car began to steer tomards the reference trajectory (decreased CTE), it will be aware of it and start to counter-steer. This is done by the differential term, wich goes smaller as the car drives has smaller CTE. This will avoid the over-shooting previously caused by the P controller alone.

## Initializing PID Controller

Proportionnal error P is initialized to a high value. Because, at the beginning, it is assumed the car is far from reference trajectory (high CTE), since we never know where a car position may start with respect to reference trajectory.

Integral erro I is initilized to 0. Because, at the beginning, there is not a sum of CTE indicating a drift correction to be made with respect to reference trajectory, i.e. there is not long term steering correction to be performed.

Differential error D is initialized to 0. Because, at the beginning, it is assumed the car is far from reference trajectory (high CTE), so there is not counter steer and make any correction.

## My PID Controller Hyperparameters P, I, D

First thing is that in the simulator there is not systematic bias, so there is no need to have the integral term, which I used 0.
Then I started trial and error to gain a better feeling how the hyperparameters P and D behave in the project track.
Here is my most importants logs:
	P, I, D
	0.05 0.0 0.0 - Initial guess, looks like proportion compensation (P) is not agressive enough to make curves.
	0.1 0.0 0.0  - P controller increased, oscillation looks fast enough but it is badly overshooting, it needs to adjust D controller.
	0.1 0.0 0.5  - D controller increased, it drives very well for Throttle = 0.3. Let's try higher speeds.
	0.1 0.0 0.5  - Tried the same hyperparameters for Throttle = 0.5, but noted that faster we go, less P controller we need, let's play with that.
	0.06 0.0 0.5 - As expected, decreasing P for faster speed works. It drives ok for Throttle = 0.5.
				   The funny thing here is that in my computer I was running the simulator using window 800x600 in "simple" mode,
				   and it was doing ok, but when I turned on the software to record my screen it started to fail. Then I turned off the software to record
				   and the model was back to normal and doing ok. So my guess is that the software recording was making the steering calculation slower.
				   I decided to improve the runtime by using the window 800x600 in "fast" mode, and then I could make it fine even with the software
				   to record the screen turned on. Interesting finding.
	0.03 0.0 0.5 - I did several trials for Throttle = 0.7 by reducing P controller, but for this track my car had a high average speed for Throttle=0.5 already.
				   My bet is that I may increase speed in straight sections of the track, but it would require to PID control the speed as well.
				   I will not implement it at this time, and will revisit the project in the future for futher improvements.
