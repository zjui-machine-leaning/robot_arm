% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly executed under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 916.568741939099596 ; 918.250221579294703 ];

%-- Principal point:
cc = [ 646.964754161949827 ; 354.577544423009385 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ 0.080097633447971 ; -0.156137351907422 ; 0.000211092335228 ; 0.007499490682131 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 11.091568375519202 ; 10.789522878389693 ];

%-- Principal point uncertainty:
cc_error = [ 7.601245841658291 ; 8.146391265683329 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.018929845037827 ; 0.064151571429157 ; 0.002702938177365 ; 0.002811317117424 ; 0.000000000000000 ];

%-- Image size:
nx = 1280;
ny = 720;


%-- Various other variables (may be ignored if you do not use the Matlab Calibration Toolbox):
%-- Those variables are used to control which intrinsic parameters should be optimized

n_ima = 5;						% Number of calibration images
est_fc = [ 1 ; 1 ];					% Estimation indicator of the two focal variables
est_aspect_ratio = 1;				% Estimation indicator of the aspect ratio fc(2)/fc(1)
center_optim = 1;					% Estimation indicator of the principal point
est_alpha = 0;						% Estimation indicator of the skew coefficient
est_dist = [ 1 ; 1 ; 1 ; 1 ; 0 ];	% Estimation indicator of the distortion coefficients


%-- Extrinsic parameters:
%-- The rotation (omc_kk) and the translation (Tc_kk) vectors for every calibration image and their uncertainties

%-- Image #1:
omc_1 = [ 1.458582e+00 ; 2.747403e+00 ; -7.742434e-03 ];
Tc_1  = [ -1.559753e+02 ; -1.043436e+02 ; 6.683346e+02 ];
omc_error_1 = [ 4.464805e-03 ; 9.477351e-03 ; 1.611206e-02 ];
Tc_error_1  = [ 5.592034e+00 ; 6.015825e+00 ; 8.204700e+00 ];

%-- Image #2:
omc_2 = [ 2.169745e+00 ; 2.200011e+00 ; -6.845300e-03 ];
Tc_2  = [ 1.491857e+01 ; -9.645733e+01 ; 6.703916e+02 ];
omc_error_2 = [ 6.830953e-03 ; 6.655229e-03 ; 1.590694e-02 ];
Tc_error_2  = [ 5.581747e+00 ; 5.930573e+00 ; 7.845807e+00 ];

%-- Image #3:
omc_3 = [ -1.431611e+00 ; -2.412097e+00 ; -4.338993e-01 ];
Tc_3  = [ -1.014660e+02 ; -1.787902e+02 ; 5.192568e+02 ];
omc_error_3 = [ 5.073683e-03 ; 7.577654e-03 ; 1.191388e-02 ];
Tc_error_3  = [ 4.395646e+00 ; 4.708569e+00 ; 6.990221e+00 ];

%-- Image #4:
omc_4 = [ -1.971434e+00 ; -1.880308e+00 ; 5.503946e-01 ];
Tc_4  = [ -1.801729e+02 ; 2.539079e+01 ; 6.486391e+02 ];
omc_error_4 = [ 9.343972e-03 ; 6.287794e-03 ; 1.161861e-02 ];
Tc_error_4  = [ 5.454593e+00 ; 5.918151e+00 ; 7.527296e+00 ];

%-- Image #5:
omc_5 = [ 2.122788e+00 ; 1.851115e+00 ; 3.168986e-01 ];
Tc_5  = [ -2.096904e+02 ; 2.645222e+01 ; 5.750367e+02 ];
omc_error_5 = [ 7.512155e-03 ; 5.556913e-03 ; 1.123212e-02 ];
Tc_error_5  = [ 4.932322e+00 ; 5.271486e+00 ; 7.207645e+00 ];

