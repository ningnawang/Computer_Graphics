/*
 * Implementations for Joint Based Characters.
 *
 * Started on October 29th, 2015 by Bryce Summers.
 */

#include "character.h"

#include "GL/glew.h"

namespace CMU462
{
   bool Joint :: calculateAngleGradient( Joint* goalJoint, Vector2D p, Vector2D q )
   {
      // TODO IMPLEMENT ME (TASK 2A)

      // x coordinate is point right, y coordinate is pointing down
      // so theta is rotating clockwisely! therefore the rotation matrix is:
      // R = (cos_theta, -sin_theta; sin_theta, cos_theta)

      // current joint is goalJoint
      if (this == goalJoint) {
         // r = (pi - pj) = (p - goalJoint->currentCenter)
         Vector2D temp = (p - goalJoint->currentCenter);
         Vector2D p_deriv = Vector2D(-temp.y, temp.x);
         goalJoint->ikAngleGradient = dot(q - p, p_deriv);

         // set all ikAngleGradient of goalJoint's children to be 0.0
         std::vector<Joint *> & goalJoint_children = goalJoint->kids;
         for (std::vector<Joint *>::iterator it = goalJoint_children.begin(); it != goalJoint_children.end(); it++) {
            (*it)->ikAngleGradient = 0.0;
         }

         return true;
      }

      // check all children of current joint
      std::vector<Joint*> & children = this->kids;
      for (std::vector<Joint*>::iterator it = children.begin(); it != children.end(); it++) {
         if ((*it)->calculateAngleGradient(goalJoint, p, q)) {
            // r = (pi - pj) = (*it)->currentCenter - this->currentCenter
            Vector2D temp = (p - this->currentCenter);
            Vector2D p_deriv = Vector2D(-temp.y, temp.x);

            this->ikAngleGradient = dot((q - p), p_deriv);
            return true;
         }
      }

      return false;
   }

   void Character :: reachForTarget( Joint* goalJoint,
                                     Vector2D sourcePoint,
                                     Vector2D targetPoint,
                                     double time )
   {
      // TODO IMPLEMENT ME (TASK 2B)

      this->update(time);

      Vector3D sourcePoint_transform = goalJoint->currentTransformation * Vector3D(sourcePoint.x, sourcePoint.y, 1);
      root->calculateAngleGradient(goalJoint, Vector2D(sourcePoint_transform.x, sourcePoint_transform.y), targetPoint);
      root->ikAngleGradient = 0.0;

      // apply gradient descent
      for (int i = 0; i < 10; i++) {
         for (std::vector<Joint*>::iterator it = joints.begin(); it != joints.end(); it++) {
            double currAngle = (*it)->getAngle(time);
            double newAngle = currAngle - 0.0000001 * (*it)->ikAngleGradient;
            (*it)->setAngle(time, newAngle);
         }  
      }


   }

   void Joint :: integrate( double time, double timestep, Vector2D cumulativeAcceleration )
   {
      // TODO IMPLEMENT ME (TASK 3A)
      double m;
      double I;
      Vector2D center_of_mass;
      this->physicalQuantities(m, I, center_of_mass, this->center); 
      double L = sqrt(dot((center_of_mass - this->center), (center_of_mass - this->center))) / 5.0;
      double g = 30.0;


      // pass the cumulative acceleration to all children
      std::vector<Joint *> children = this->kids;
      for (std::vector<Joint *>::iterator it = children.begin(); it != children.end(); it++) {

         // calculate keyframe rotation accelaration
         if (this->type == KEYFRAMED) {
            // Lnew is different from L
            double Lnew = sqrt(dot((this->center - (*it)->center), (this->center - (*it)->center))) / 5.0;
            double alpha = angle.evaluate(time);
            double alpha_1stDer = angle.evaluate(time, 1);
            double alpha_2ndDer = angle.evaluate(time, 2);

            double sin_alpha = sinf(alpha);
            double cos_alpha = cosf(alpha);

            double tempX = Lnew * (-sinf(alpha) * pow(alpha_1stDer, 2) + alpha_2ndDer * cosf(alpha)) * 5.0;
            double tempY = Lnew * (cosf(alpha) * pow(alpha_1stDer, 2) + alpha_2ndDer * sinf(alpha)) * 5.0;

            Vector3D a_rotation_homo = Vector3D(tempX, tempY, 0); // not do (x, y, 1), coz we don't want translation here
            a_rotation_homo = currentParentTransformation * a_rotation_homo;
            Vector2D a_rotation = Vector2D(a_rotation_homo.x, a_rotation_homo.y);

            cumulativeAcceleration = cumulativeAcceleration + a_rotation;
         }

         (*it)->integrate(time, timestep, cumulativeAcceleration);
      }


      // apply equation of motion
      double current_omega = this->omega;
      double current_theta = this->theta;

      // calculate psi responsing to center of mass
      Vector2D y = center_of_mass - this->center;
      double psi = atan2(y.x, y.y); //radian
      current_theta = current_theta - psi;
      Vector2D y_perp = Vector2D(-y.y, y.x);

      // using semi-implicit Euler
      // apply damping by multiplying exp(-a * timestep) where a = 1
      // also consider swing motion
      double temp_step = dot(cumulativeAcceleration, y_perp) / pow(L, 2) +  m * g * L * sinf(current_theta) * timestep / I;
      this->omega = (current_omega - temp_step) * exp(-timestep);
      this->theta = current_theta + this->omega * timestep;

   }

   void Character :: integrate( double time, double timestep )
   {
      // TODO IMPLEMENT ME (TASK 3B)

      //calculate translation accelaration
      Vector2D a_translation = position.evaluate(time, 2);

      std::vector<Joint *> children = root->kids;
      for (std::vector<Joint *>::iterator it = children.begin(); it != children.end(); it++) {
         (*it)->integrate(time, timestep, a_translation);
      }

   }

   void Character :: update( double time )
   {
      currentTransformation = Matrix3x3::translation( position.evaluate( time ) );
      root->update( time, currentTransformation );
   }

   void Character::draw( SVGRenderer* renderer, bool pick, Joint* hovered, Joint* selected )
   {
      root->draw( renderer, pick, hovered, selected );
   }

   // Every Joint is grouped with a circle representing the center.
   // The SVG file contains a hieracrhy of groups cooresponding to
   void Character::load_from_SVG(SVG & svg)
   {
      Group * root_group = static_cast<Group*>(svg.elements[0]);

      joints.clear();
      root = new Joint();
      root->index = joints.size();
      joints.push_back(root);

      root->parse_from_group(root_group, *this);
   }

   // The constructor sets the dynamic angle and velocity of
   // the joint to zero (at a perfect vertical with no motion)
   Joint :: Joint( void )
   : theta( 0. ), omega( 0. )
   {}

   Joint :: ~Joint( void )
   {
      // deallocate storage of SVG shapes
      for( int i = 0; i < shapes.size(); i++ )
      {
         delete shapes[i];
      }
   }

   double Joint::getAngle( double time )
   {
      if( type == DYNAMIC )
      {
         return theta;
      }

      // type == KEYFRAMED
      return angle( time );
   }

   void Joint::setAngle( double time, double value )
   {
      if( type == DYNAMIC )
      {
         theta = value;
         return;
      }

      // type == KEYFRAMED
      angle.setValue( time, value );
   }

   bool Joint::removeAngle(double time)
   {
      // Assuming times are on the integers only.
      return angle.removeKnot(time, .1);
   }

   void Joint::resetVelocity( void )
   {
      omega = 0.;
   }

   void Joint::resetDynamics( void )
   {
      theta = 0.;
      omega = 0.;
   }

   void Joint::update( double time, Matrix3x3 parentTransformation )
   {
      // Calculate the cumulative transformation by composing the
      // transformation of the parent with a rotation around the
      // joint center.

      double alpha = getAngle( time );
      if( type == DYNAMIC )
      {
         // A pendulum should hang straight down; its angle should
         // not be affected by the rotation of any joints above it.
         alpha -= parentTransformation.getRotation();
      }

      Matrix3x3 R = Matrix3x3::rotation( alpha );
      Matrix3x3 A = Matrix3x3::translation(  center );
      Matrix3x3 B = Matrix3x3::translation( -center );
      currentParentTransformation = parentTransformation;
      currentTransformation = parentTransformation * A * R * B;

      Vector3D c( center.x, center.y, 1. );
      c = currentTransformation * c;
      currentCenter = Vector2D( c.x/c.z, c.y/c.z );

      for( vector<Joint*>::iterator joint  = kids.begin(); joint != kids.end(); joint ++ )
      {
         (*joint)->update( time, currentTransformation );
      }
   }

   inline double mod1(double in) { return in > 1.0 ? in - 1 : in;}


   void Joint::draw( SVGRenderer* renderer, bool pick, Joint* hovered, Joint* selected )
   {
      for( vector<SVGElement*>::iterator shape = shapes.begin(); shape != shapes.end(); shape++ )
      {
         // make a copy of the original style for this shape,
         // so that we can restore it if it's changed for either
         // picking or hovering/selection
         Style originalStyle = (*shape)->style;

         if( pick )
         {
            // compute and apply the picking color
            Color pickColor = Color::fromPickIndex( index+1 );
            (*shape)->style.strokeColor = pickColor;
            (*shape)->style.fillColor   = pickColor;
         }
         else if( this == selected )
         {
            // highlight the fill and stroke color
            // Selected joints are drawn with a contrasting color.
            Color& fillColor( (*shape)->style.fillColor );
            fillColor.r = mod1(fillColor.r + .5);
            fillColor.g = mod1(fillColor.g + .5);
            fillColor.b = mod1(fillColor.b + .5);


            Color& strokeColor( (*shape)->style.fillColor );
            /*
               fillColor.r = 1.;
               fillColor.g = 1.;
               fillColor.b = 1.;
               */
         }
         else if( this == hovered )
         {
            // highlight the fill color
            Color& fillColor( (*shape)->style.fillColor );
            fillColor.r += .35;
            fillColor.g += .35;
            fillColor.b += .35;
         }

         renderer->pushTransformation();
         renderer->concatenateTransformation( currentTransformation );
         renderer->draw_element( *shape );
         renderer->popTransformation();

         // restore the original style
         (*shape)->style = originalStyle;
      }

      for( vector<Joint*>::iterator joint = kids.begin(); joint != kids.end(); joint ++ )
      {
         (*joint)->draw( renderer, pick, hovered, selected );
      }
   }

   void Joint::parse_from_group(Group * G, Character & C)
   {
      // Attempt to parse the starting group containing the current joint's data.
      vector<SVGElement*> & elements = G -> elements;
      vector<SVGElement*>::iterator iter = elements.begin();

      // Parse this particular group's data.
      Group * joint_group = dynamic_cast<Group*>(*iter);

      // Base Case, this is childless joint if this is not a group.
      if(joint_group == NULL)
      {
         // one shape for each element,
         //excluding the final circle which just specifies the joint
         int nShapes = elements.size()-1;
         shapes.resize( nShapes );
         for( int i = 0; i < nShapes; i++ )
         {
            shapes[i] = (*iter)->copy();
            iter++;
         }
         Circle * center_circle =  static_cast<Circle*>(*iter);
         center = center_circle -> center;
         setJointType( center_circle );
         return;
      }

      // one shape for each element,
      // excluding the final circle which just specifies the joint.
      int nShapes = joint_group->elements.size()-1; 
      shapes.resize( nShapes );
      for( int i = 0; i < nShapes; i++ )
      {
         shapes[i] = ((joint_group->elements)[i])->copy();
      }
      SVGElement * svg_circle = (joint_group->elements)[nShapes];
      Circle * center_circle  = dynamic_cast<Circle*>(svg_circle);

      if(center_circle == NULL)
      {
         cout << svg_circle << endl;
         cout << svg_circle->type << endl;
         cerr << "ERROR: The Center of rotation circle was not found during joint parsing.\n";
         exit(0);
      }

      center = center_circle -> center;
      setJointType( center_circle );

      iter++;


      // Parse the sub joints.
      while(iter != elements.end())
      {
         Joint * child = new Joint();

         // Allocate new joints for the children in the Character's joints vector.
         child->index = C.joints.size();
         C.joints.push_back(child);

         // Link each of these joints to this joint's kids array.
         kids.push_back(child);

         Group * child_group = static_cast<Group*>(*iter);

         // Parse the child.
         child -> parse_from_group(child_group, C);

         // Continue on to the next child.
         iter++;
      }

      // Done parsing subjoints.

   }

   void Joint :: setJointType( Circle* circle )
   {
      Color& c( circle->style.fillColor );

      type = KEYFRAMED;

      if( c.r == 1. && c.g == 1. && c.b == 1. )
      {
         type = DYNAMIC;
      }
   }

   void Joint :: physicalQuantities( double& m, // mass
                                     double& I, // moment of inertia
                                     Vector2D& c, // centroid
                                     Vector2D center
                                     ) const
   {
      m = 0.;
      I = 0.;
      c = Vector2D( 0., 0. );

      for( int k = 0; k < shapes.size(); k++ )
      {
         double   mk = shapes[k]->mass();
         double   Ik = shapes[k]->momentOfInertia( center );
         Vector2D ck = shapes[k]->centroid();

         m += mk;
         I += Ik;
         c += mk*ck;
      }

      c /= m;
   }
}
