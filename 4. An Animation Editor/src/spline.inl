// Given a time between 0 and 1, evaluates a cubic polynomial with
// the given endpoint and tangent values at the beginning (0) and
// end (1) of the interval.  Optionally, one can request a derivative
// of the spline (0=no derivative, 1=first derivative, 2=2nd derivative).
template <class T>
inline T Spline<T>::cubicSplineUnitInterval(
      const T& position0,
      const T& position1,
      const T& tangent0,
      const T& tangent1,
      double normalizedTime,
      int derivative )
{
   // TODO IMPLEMENT ME (TASK 1A)

   double t = normalizedTime;
   double h00, h10, h01, h11;

   if (derivative == 0) { // no derivative
      h00 = 2 * pow(t, 3) - 3 * pow(t, 2) + 1;
      h10 = pow(t, 3) - 2 * pow(t, 2)+ t;
      h01 = -2 * pow(t, 3) + 3 * pow(t, 2);
      h11 = pow(t, 3) - pow(t, 2);
   }
   else if (derivative == 1) { // first derivative
      h00 = 6 * pow(t, 2) - 6 * t;
      h10 = 3 * pow(t, 2) - 4 * t + 1;
      h01 = -6 * pow(t, 2) + 6 * t;
      h11 = 3 * pow(t, 2) - 2 * t;
   }
   else if (derivative == 2) { // seconde derivative
      h00 = 12 * t - 6;
      h10 = 6 * t - 4;
      h01 = -12 * t + 6;
      h11 = 6 * t - 2;
   }
   else {
      return T();
   }

   T result = h00 * position0 + h10 * tangent0 + h01 * position1 + h11 * tangent1;
   return result;

}

template <class T>
inline T Spline<T>::cubicSplineUnitInterval_Centripetal( const T& P0,
                                                         const T& P1,  
                                                         const T& P2,
                                                         const T& P3,
                                                         double normalizedTime,
                                                         int derivative ) {
   double t = normalizedTime;
   double L = sqrt(dot(P1 - P0, P1 - P0)) + sqrt(dot(P2 - P1, P2 - P1)) + sqrt(dot(P3 - P2, P3 - P2));
   double t0 = 0.0;
   double t1 = (sqrt(dot(P1 - P0, P1 - P0)) + t0) / L;
   double t2 = (sqrt(dot(P2 - P1, P2 - P1)) + t1) / L;
   double t3 = 1.0;

   std::cout << "t1: " << t1 << std::endl;
   std::cout << "t2: " << t2 << std::endl;

   T B1, B2, A1, A2, A3, C;
   T B11, B21, A11, A21, A31;
   T B12, B22;

   if (derivative == 0) { // no derivative

      /** Centripetal Catmull–Rom spline **/
      A1 = (t1-t)/(t1-t0)*P0 + (t-t0)/(t1-t0)*P1;
      A2 = (t2-t)/(t2-t1)*P1 + (t-t1)/(t2-t1)*P2;
      A3 = (t3-t)/(t3-t2)*P2 + (t-t2)/(t3-t2)*P3;

      B1 = (t2-t)/(t2-t0)*A1 + (t-t0)/(t2-t0)*A2;
      B2 = (t3-t)/(t3-t1)*A2 + (t-t1)/(t3-t1)*A3;

      C  = (t2-t)/(t2-t1)*B1 + (t-t1)/(t2-t1)*B2;
   }
   else if (derivative == 1 || derivative == 2) { // first derivative + seconde derivative
      A11 = (P1 - P0) * 1.0 / (t1 - t0);
      A21 = (P2 - P1) * 1.0 / (t2 - t1);
      A31 = (P3 - P2) * 1.0 / (t1 - t0);

      B11 = (t2-t)/(t2-t0)*A11 + (t-t0)/(t2-t0)*A21 + (A2 - A1) * 1.0 / (t2 - t0);
      B21 = (t3-t)/(t3-t1)*A21 + (t-t1)/(t3-t1)*A31 + (A3 - A2) * 1.0 / (t3 - t1);

      C  = (t2-t)/(t2-t1)*B11 + (t-t1)/(t2-t1)*B21 + (B2 - B1) * 1.0 / (t2 - t1);

   }
   else {
      return T();
   }

   return C;

}



            
// Returns a state interpolated between the values directly before and after the given time.
template <class T>
inline T Spline<T>::evaluate( double time, int derivative )
{
   // TODO IMPLEMENT ME (TASK 1B)

   // no knots in the spline
   if (knots.size() < 1) {
      return T();
   }

   typename std::map<double,T>::iterator itBegin = knots.begin();
   typename std::map<double,T>::iterator itEnd = prev(knots.end());

   // only one knot in the spline
   if (knots.size() == 1) {
      itEnd = knots.begin();
      if (derivative == 0) {
         return itBegin->second;
      }
      else {
         return T();
      }
   }

   // query time <= initial knot
   if (time <= itBegin->first) {
      if (derivative == 0) {
         return itBegin->second;
      }
      else {
         return T();
      }
   }

   // query time >= final knot
   if (time >= itEnd->first) {
      if (derivative == 0) {
         return itEnd->second;
      }
      else {
         return T();
      }
   }

   typename std::map<double, T>::iterator t2_iter = knots.upper_bound(time); // t2 > time
   typename std::map<double, T>::iterator t1_iter = prev(t2_iter);

   double t1 = t1_iter->first;
   double t2 = t2_iter->first;
   T q1 = t1_iter->second;
   T q2 = t2_iter->second;
   double t0, t3;
   T q0, q3;

   // mirroring left
   if (t1_iter == itBegin) {
      t0 = t1 - (t2 - t1);
      // t0 = t1;
      q0 = q1 - (q2 - q1);
   }
   else {
      typename std::map<double, T>::iterator t0_iter = prev(t1_iter);
      t0 = t0_iter->first;
      q0 = t0_iter->second;
   }

   //mirroring right
   if (t2_iter == itEnd) {
      t3 = t2 + (t2 - t1);
      // t3 = t2;
      q3 = q2 + (q2 - q1);
   }
   else {
      typename std::map<double, T>::iterator t3_iter = next(t2_iter);
      t3 = t3_iter->first;
      q3 = t3_iter->second;
   }

   T v1 = (q2 - q0) * (1.0 / (t2 - t0));
   T v2 = (q3 - q1) * (1.0 / (t3 - t1));

   // /** Centripetal Catmull–Rom spline **/
   // t1 = sqrt(dot(q1 - q0, q1 - q0)) + t0;
   // t2 = sqrt(dot(q2 - q1, q2 - q1)) + t1;
   // t3 = sqrt(dot(q3 - q2, q3 - q2)) + t2;


   // transfer from global spline q(t) to local spline p(t)
   // p(t) = q(t_i + t(t_i+1 - t_i), t in [0, 1]
   // so p(0) = q1, p(1) = q2
   // normalize time to [0, 1] based on current interval 
   double t = (time - t1) * (1.0 / (t2 - t1));
   T m1 = (t2 - t1) * v1;
   T m2 = (t2 - t1) * v2;

   if (derivative == 1) {
      return cubicSplineUnitInterval(q1, q2, m1, m2, t, derivative) * (1.0 / (t2 - t1));
      // return cubicSplineUnitInterval_Centripetal(q0, q1, q2, q3, t, 1) * (1.0 / (t2 - t1));
   }
   else if (derivative == 2) {
      return cubicSplineUnitInterval(q1, q2, m1, m2, t, derivative) * (1.0 / pow((t2 - t1), 2));
      // return cubicSplineUnitInterval_Centripetal(q0, q1, q2, q3, t, 2) * (1.0 / pow((t2 - t1), 2));
   }
   // derivative == 0
   return cubicSplineUnitInterval(q1, q2, m1, m2, t, derivative);
   // return cubicSplineUnitInterval_Centripetal(q0, q1, q2, q3, t, 0);
}

// Removes the knot closest to the given time,
//    within the given tolerance..
// returns true iff a knot was removed.
template <class T>
inline bool Spline<T>::removeKnot(double time, double tolerance )
{
   // Empty maps have no knots.
   if( knots.size() < 1 )
   {
      return false;
   }

   // Look up the first element > or = to time.
   typename std::map<double, T>::iterator t2_iter = knots.lower_bound(time);
   typename std::map<double, T>::iterator t1_iter;
   t1_iter = t2_iter;
   t1_iter--;

   if( t2_iter == knots.end() )
   {
      t2_iter = t1_iter;
   }

   // Handle tolerance bounds,
   // because we are working with floating point numbers.
   double t1 = (*t1_iter).first;
   double t2 = (*t2_iter).first;

   double d1 = fabs(t1 - time);
   double d2 = fabs(t2 - time);


   if(d1 < tolerance && d1 < d2)
   {
      knots.erase(t1_iter);
      return true;
   }

   if(d2 < tolerance && d2 < d1)
   {
      knots.erase(t2_iter);
      return t2;
   }

   return false;
}

// Sets the value of the spline at a given time (i.e., knot),
// creating a new knot at this time if necessary.
template <class T>
inline void Spline<T>::setValue( double time, T value )
{

   knots[ time ] = value;
}

template <class T>
inline T Spline<T>::operator()( double time )
{
   return evaluate( time );
}
