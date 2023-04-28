(define (problem ARC-problem2)
    (:domain ARC)

    (:objects
        c0 c1 - color
        output_pixels_1_1 output_pixels_1_2 output_pixels_1_3 output_pixels_1_4 output_pixels_1_5 output_pixels_2_1 output_pixels_2_2 output_pixels_2_3 output_pixels_2_4 output_pixels_2_5 output_pixels_3_1 output_pixels_3_2 output_pixels_3_3 output_pixels_3_4 output_pixels_3_5 output_pixels_4_1 output_pixels_4_2 output_pixels_4_3 output_pixels_4_4 output_pixels_4_5 output_pixels_5_1 output_pixels_5_2 output_pixels_5_3 output_pixels_5_4 output_pixels_5_5 - output_pixels
    )

    (:init
        (color_usable c0)
        (color_usable c1)
        (painted_output output_pixels_2_5 c1)
        (painted_output output_pixels_3_2 c1)
        (painted_output output_pixels_5_4 c1)
        (painted_output output_pixels_2_1 c1)
        (painted_output output_pixels_5_3 c1)
        (painted_output output_pixels_1_3 c1)
        (painted_output output_pixels_3_1 c1)
        (painted_output output_pixels_3_3 c0)
        (painted_output output_pixels_3_4 c1)
        (painted_output output_pixels_3_5 c1)
        (painted_output output_pixels_1_1 c1)
        (painted_output output_pixels_5_2 c1)
        (painted_output output_pixels_5_5 c1)
        (painted_output output_pixels_4_4 c1)
        (painted_output output_pixels_2_2 c1)
        (painted_output output_pixels_1_2 c1)
        (painted_output output_pixels_4_5 c1)
        (painted_output output_pixels_1_4 c1)
        (painted_output output_pixels_1_5 c1)
        (painted_output output_pixels_2_4 c1)
        (painted_output output_pixels_4_3 c1)
        (painted_output output_pixels_5_1 c1)
        (painted_output output_pixels_4_2 c1)
        (painted_output output_pixels_4_1 c1)
        (painted_output output_pixels_2_3 c1)
    )

    (:goal
        (and (painted_output output_pixels_1_1 c0) (painted_output output_pixels_1_2 c1) (painted_output output_pixels_1_3 c1) (painted_output output_pixels_1_4 c1) (painted_output output_pixels_1_5 c0) (painted_output output_pixels_2_1 c1) (painted_output output_pixels_2_2 c0) (painted_output output_pixels_2_3 c1) (painted_output output_pixels_2_4 c0) (painted_output output_pixels_2_5 c1) (painted_output output_pixels_3_1 c1) (painted_output output_pixels_3_2 c1) (painted_output output_pixels_3_3 c0) (painted_output output_pixels_3_4 c1) (painted_output output_pixels_3_5 c1) (painted_output output_pixels_4_1 c1) (painted_output output_pixels_4_2 c0) (painted_output output_pixels_4_3 c1) (painted_output output_pixels_4_4 c0) (painted_output output_pixels_4_5 c1) (painted_output output_pixels_5_1 c0) (painted_output output_pixels_5_2 c1) (painted_output output_pixels_5_3 c1) (painted_output output_pixels_5_4 c1) (painted_output output_pixels_5_5 c0))
    )

    
    
    
)