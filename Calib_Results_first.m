% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly executed under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 883.345728102971066 ; 889.173316053536041 ];

%-- Principal point:
cc = [ 635.605930031935259 ; 357.159606632066073 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ 0.043209427926409 ; -0.079434782524832 ; 0.009776937333058 ; 0.004746588285449 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 13.504770914050894 ; 13.226204106727989 ];

%-- Principal point uncertainty:
cc_error = [ 8.076882304447020 ; 9.495100555087525 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.018380430176079 ; 0.055982600492388 ; 0.003632210864271 ; 0.003149132776867 ; 0.000000000000000 ];

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
omc_1 = [ 2.992591e+00 ; 9.198523e-01 ; 6.077723e-02 ];
Tc_1  = [ -7.668622e+01 ; 1.918032e+01 ; 6.470676e+02 ];
omc_error_1 = [ 1.037944e-02 ; 2.895672e-03 ; 1.625058e-02 ];
Tc_error_1  = [ 5.940366e+00 ; 6.917686e+00 ; 9.808257e+00 ];

%-- Image #2:
omc_2 = [ -2.210249e+00 ; -2.236470e+00 ; -4.904427e-03 ];
Tc_2  = [ 1.819225e+02 ; -1.017775e+02 ; 6.520727e+02 ];
omc_error_2 = [ 7.202295e-03 ; 1.077479e-02 ; 1.974329e-02 ];
Tc_error_2  = [ 6.117788e+00 ; 7.080548e+00 ; 1.031933e+01 ];

%-- Image #3:
omc_3 = [ -2.619012e+00 ; -6.683908e-01 ; 3.676521e-03 ];
Tc_3  = [ -5.556633e+00 ; -5.994112e+01 ; 5.668397e+02 ];
omc_error_3 = [ 8.879911e-03 ; 3.392475e-03 ; 1.109404e-02 ];
Tc_error_3  = [ 5.197329e+00 ; 6.075195e+00 ; 8.769019e+00 ];

%-- Image #4:
omc_4 = [ -1.969450e+00 ; -1.878045e+00 ; 5.277087e-01 ];
Tc_4  = [ -1.439717e+01 ; 2.022963e+01 ; 6.343140e+02 ];
omc_error_4 = [ 8.815268e-03 ; 6.954609e-03 ; 1.503644e-02 ];
Tc_error_4  = [ 5.786171e+00 ; 6.778101e+00 ; 8.595904e+00 ];

%-- Image #5:
omc_5 = [ 2.131629e+00 ; 1.857363e+00 ; 3.071332e-01 ];
Tc_5  = [ -4.336148e+01 ; 2.190455e+01 ; 5.643614e+02 ];
omc_error_5 = [ 9.356346e-03 ; 4.918530e-03 ; 1.344489e-02 ];
Tc_error_5  = [ 5.175952e+00 ; 6.023543e+00 ; 8.298946e+00 ];

