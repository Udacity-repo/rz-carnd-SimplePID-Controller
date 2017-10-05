#include "PID.h"
#include <iostream>
#include <limits>

using namespace std;

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
  this->Kp = Kp; // Proportionnal term
  this->Ki = Ki; // Integral term 
  this->Kd = Kd; // Differential term

  // Init the Proportionnal error to a high value
  // Because, at the beginning, it is assumed the car is far from reference trajectory (high CTE) 
  // since we never know where a car position may start with respect to reference trajectory
  p_error = numeric_limits<double>::max();  

  // Init the Integral term to 0
  // Because, at the beginning, there is not a sum of CTE indicating a drift correction to be made with respect to reference trajectory 
  // i.e. there is not long term steering correction to be performed
  i_error = 0.0;

  // Init the Differential error to 0
  // Because, at the beginning, it is assumed the car is far from reference trajectory (high CTE)
  // so there is not counter steer and make any correction
  d_error = 0.0; 
}

void PID::UpdateError(double cte) {
  if (p_error == numeric_limits<double>::max())
    p_error = cte;
  d_error = cte - p_error;  //differencial error (short term steering correction)
  p_error = cte; // proportional error (steering estimate)
  i_error += cte; // integral error (long term steering correction)
}

double PID::TotalError() {
  // -tau_p * CTE - tau_d * diff_CTE - tau_i * int_CTE
  return Kp * p_error + Kd * d_error + Ki * i_error;
}

