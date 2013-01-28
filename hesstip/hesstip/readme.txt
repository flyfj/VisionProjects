This directory contains applications based on the ECCV'08 paper:

     An Efficient Dense and Scale-Invariant Spatio-Temporal Interest Point Detector
     Geert Willems, Tinne Tuytelaars and Luc Van Gool


There are currently 3 applications included:
- HesSTIPFromVideo : takes a video as input and computes the features that
  		     are invariant in space and time
- HesSTIPFixedTauFromVideo : works similar but uses a fixed, user-defined
			     temporal scale
- featureViewer : the resulting data can be visualized in this viewer


All applications are set to reasonable default settings.
A sample video, taken from the KTH dataset, has been included.

Besides the two included .so files, the following libraries are needed:
- Qt (4.4)
- OpenCV (1.0.0)
- vxl (1.7.0)
- boost 
All applications are compiled on 64bit Fedora9


For more information or requests for other builds, please contact
geert.willems@esat.kuleuven.be
