##
## Author(s):
##  - Cedric GESTES      <gestes@aldebaran-robotics.com>
##  - Herve Cuche        <hcuche@aldebaran-robotics.com>
##
## Copyright (C) 2009, 2010, 2012 Aldebaran Robotics
##

project (QiService)

qi_create_bin(qi-service
  qiservicetestmain.cpp
  DEPENDS QI BOOST_PROGRAM_OPTIONS)
