(define (problem ARC-problem1)
    (:domain ARC)

    (:objects
        c0 c1 - color
        output_pixels_1_1 output_pixels_1_2 output_pixels_1_3 output_pixels_2_1 output_pixels_2_2 output_pixels_2_3 output_pixels_3_1 output_pixels_3_2 output_pixels_3_3 - output_pixels
    )

    (:init
        (color_usable c0)
        (color_usable c1)
        (painted_output output_pixels_3_1 c1)
        (painted_output output_pixels_1_2 c1)
        (painted_output output_pixels_3_2 c1)
        (painted_output output_pixels_2_1 c1)
        (painted_output output_pixels_2_2 c0)
        (painted_output output_pixels_3_3 c1)
        (painted_output output_pixels_1_3 c1)
        (painted_output output_pixels_1_1 c1)
        (painted_output output_pixels_2_3 c1)
    )

    (:goal
        (and (painted_output output_pixels_1_1 c0) (painted_output output_pixels_1_2 c1) (painted_output output_pixels_1_3 c0) (painted_output output_pixels_2_1 c1) (painted_output output_pixels_2_2 c0) (painted_output output_pixels_2_3 c1) (painted_output output_pixels_3_1 c0) (painted_output output_pixels_3_2 c1) (painted_output output_pixels_3_3 c0))
    )

    
    
    
)