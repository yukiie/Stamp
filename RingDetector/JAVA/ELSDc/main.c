/*------------------------------------------------------------------------------
 
 Copyright (c) 2007-2011 rafael grompone von gioi (grompone@gmail.com)
 Copyright (c) 2012-2014 viorica patraucean (vpatrauc@gmail.com)
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as
 published by the Free Software Foundation, either version 3 of the
 License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Affero General Public License for more details.
 
 You should have received a copy of the GNU Affero General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 
 
 main.c - This file belongs to ELSDc project (Ellipse and Line Segment
 Detector with continuous validation)
 - It contains the main function, which reads a pgm image, calls the
 detector, and writes the result in ASCII and SVG form.
 
 ------------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include "misc.h"
#include "pgm.h"
#include "svg.h"
#include "polygon.h"
#include "ring.h"
#include "elsdc.h"
#include <math.h>


/*----------------------------------------------------------------------------*/
/** Main function; takes as argument the name of a pgm image, runs the detection
 on it, and writes the result in ASCII and SVG form.
 */
int main( int argc, char **argv )
{
    /* check arguments */
    if( argc < 2 ) error("usage: ./elsdc image_name.pgm");
    
    PImageDouble in;     /* input image */
    PImageInt    out;    /* output image having the same size as 'in'; the pixels
                          supporting a certain geometric primitive are marked
                          with the same label */
    
    int ell_count = 0;   /* number of detected ellipses */
    int *ell_labels=NULL;/* the pixels supporting a certain ellipse are marked
                          with the same unique label */
    Ring *ell_out = NULL;/* array containing the parameters of the detected
                          ellipses; correlated with ell_labels, i.e. the i-th
                          element of ell_labels is the label of the pixels
                          supporting the ellipse defined by the parameters
                          ell[i] */
    
    int poly_count = 0;  /* number of detected polygons */
    int *poly_labels=NULL;/* the pixels supporting a certain polygon are marked
                           with the same unique label */
    Polygon *poly_out=NULL;/* array containing the parameters of the detected
                            polygons; correlated with poly_labels, i.e. the i-th
                            element of ell_labels is the label of the pixels
                            supporting the polygon defined by the parameters
                            poly[i] */
    
    FILE *ell_ascii;     /* output file with the parameters of the detected
                          ellipses -- ASCII format */
    FILE *poly_ascii;    /* output file with the parameters of the detected
                          polygons -- ASCII format */
//    FILE *fsvg;          /* output file with the detected ellipses and polygons
//                          in vectorial form */
    int i,j;

    //zhj 2019.7.11
    /* read input image; must be PGM form */
    in = read_pgm_image_double( argv[1] );
    int xsize = in->xsize, ysize = in->ysize;
    
    /* create and initialize with 0 output label image */
    out = new_PImageInt_ini( in->xsize, in->ysize, 0 );
    
    /* call detection procedure */
    ELSDc( in, &ell_count, &ell_out, &ell_labels, &poly_count, &poly_out,
          &poly_labels, out );
    
    if( (ell_ascii = fopen("out_ellipse.txt","w")) == NULL )
        error("main: can't open ellipse output file.");
    if(ell_count!=0){
        for(i=0;i<ell_count;i++){
            fprintf( ell_ascii,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf \n",
                    ell_out[i].x1, ell_out[i].y1, ell_out[i].x2, ell_out[i].y2,
                    ell_out[i].cx, ell_out[i].cy, ell_out[i].ax, ell_out[i].bx,
                    ell_out[i].theta, ell_out[i].ang_start, ell_out[i].ang_end );
        }
    }
    fclose(ell_ascii);
    free_PImageInt(out);
    if( ell_out != NULL ) {free(ell_out); free(ell_labels);}
    if( poly_out != NULL )
    {
        for( i=0; i<poly_count; i++ )
            free(poly_out[i].pts);
        free(poly_out);
        free(poly_labels);
    }
    printf("Number of ellipses detected = %d\n",ell_count);
    return 0;
}


