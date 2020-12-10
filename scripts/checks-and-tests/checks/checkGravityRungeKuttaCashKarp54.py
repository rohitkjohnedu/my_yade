# -*- coding: utf-8

# 3 Spheres have an initial velociries: 0, +5, -5
# Their positions and velocities are checked during free fall
# Checks the correctness of RungeKuttaCashKarp54Integrator and GravityEngine
# Also see checkGravity.py

## Omega
from __future__ import print_function
o=Omega() 

## PhysicalParameters 
Density=2400
frictionAngle=radians(35)
sphereRadius=0.05
tc = 0.001
en = 0.3
et = 0.3

O.dt=0.02*tc

sphereMat=O.materials.append(ViscElMat(density=Density,frictionAngle=frictionAngle,tc=tc,en=en,et=et))


v_down    = -5.0
v_up      =  5.0
g         = -9.81
tolerance = 1e-12

id_0    = o.bodies.append(sphere((0,0,0),0.2,material=sphereMat)) # The body has no initial vertical Velocity
id_down = o.bodies.append(sphere((1,0,0),0.2,material=sphereMat)) # The body has an initial vertical Velocity -5
id_up   = o.bodies.append(sphere((2,0,0),0.2,material=sphereMat)) # The body has an initial vertical Velocity +5

O.bodies[id_down].state.vel[1] = v_down
O.bodies[id_up  ].state.vel[1] = v_up

# RungeKuttaCashKarp54Integrator calculates O.dt step with requested precision (rel_err, abs_err).
# The calculations involve checking forces at various positions, resetting state and so on, multiple times. With very small timesteps. But the end result is the dimulation advanced by O.dt
# see https://www.boost.org/doc/libs/1_74_0/libs/numeric/odeint/doc/html/boost/numeric/odeint/runge_kutta_cash_karp54.html
# and https://yade-dem.org/doc/yade.wrapper.html?highlight=rungekuttacashkarp54integrator#yade.wrapper.RungeKuttaCashKarp54Integrator
integrator=RungeKuttaCashKarp54Integrator([
  ForceResetter(),
  GeneralIntegratorInsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb(),]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  GravityEngine(gravity=[0,g,0]),
]);

# Tolerances can be set for the optimum accuracy. Just for fun use maximum available precision: yade.math.epsilon
integrator.rel_err = yade.math.epsilon() # 1e-20; # yade.math.epsilon()
integrator.abs_err = yade.math.epsilon() # 1e-20;

## Engines 
o.engines=[
   integrator
  ,PyRunner(command='checkPos()',iterPeriod=10),
]

def checkPos():
  print("Iter=%i, time=%.15f, dt=%.15f" % (O.iter, O.time, O.dt))
  if (abs((O.bodies[id_0   ].state.pos[1] - getCurrentPos(0     ))/O.bodies[id_0   ].state.pos[1]) > tolerance):
    warningMessagePos (0     , O.bodies[id_0   ].state.pos[1], getCurrentPos(0))

  if (abs((O.bodies[id_down].state.pos[1] - getCurrentPos(v_down))/O.bodies[id_down].state.pos[1]) > tolerance):
    warningMessagePos (v_down, O.bodies[id_down].state.pos[1], getCurrentPos(v_down))

  if (abs((O.bodies[id_up  ].state.pos[1] - getCurrentPos(v_up  ))/O.bodies[id_up  ].state.pos[1]) > tolerance):
    warningMessagePos (v_up  , O.bodies[id_up  ].state.pos[1], getCurrentPos(v_up))

  if (abs((O.bodies[id_0   ].state.vel[1] - getCurrentVel(0     ))/O.bodies[id_0   ].state.vel[1]) > tolerance):
    warningMessageVel (0     , O.bodies[id_0   ].state.vel[1], getCurrentVel(0))

  if (abs((O.bodies[id_down].state.vel[1] - getCurrentVel(v_down))/O.bodies[id_down].state.vel[1]) > tolerance):
    warningMessageVel (v_down, O.bodies[id_down].state.vel[1], getCurrentVel(v_down))

  if (abs((O.bodies[id_up  ].state.vel[1] - getCurrentVel(v_up  ))/O.bodies[id_up  ].state.vel[1]) > tolerance):
    warningMessageVel (v_up  , O.bodies[id_up  ].state.vel[1], getCurrentVel(v_up))

def getCurrentPos(inVel=0):
  t = O.time + O.dt
  return inVel*t + g*t*t/2

def getCurrentVel(inVel=0):
  t = O.time + O.dt
  return inVel + g*t

def warningMessagePos(inVel, y_pos, y_pos_need):
  raise YadeCheckError("The body with the initial velocity %.3f, has an y-position %.19f, but it should be %.19f. Iter=%i, time=%.15f, dt=%.15f" % (inVel, y_pos, y_pos_need, O.iter, O.time, O.dt))

def warningMessageVel(inVel, y_vel, y_vel_need):
  raise YadeCheckError("The body with the initial velocity %.3f, has an y-velocity %.19f, but it should be %.19f. Iter=%i, time=%.15f, dt=%.15f" % (inVel, y_vel, y_vel_need, O.iter, O.time, O.dt))

#O.saveTmp('init');
O.run(1000000,True)

