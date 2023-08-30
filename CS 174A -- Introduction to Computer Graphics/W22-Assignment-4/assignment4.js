import {defs, tiny} from './examples/common.js';

const {
    Vector, Vector3, vec, vec3, vec4, color, hex_color, Shader, Matrix, Mat4, Light, Shape, Material, Scene, Texture,
} = tiny;

const {Cube, Axis_Arrows, Textured_Phong} = defs

export class Assignment4 extends Scene {
    /**
     *  **Base_scene** is a Scene that can be added to any display canvas.
     *  Setup the shapes, materials, camera, and lighting here.
     */
    constructor() {
        // constructor(): Scenes begin by populating initial values like the Shapes and Materials they'll need.
        super();

        // TODO:  Create two cubes, including one with the default texture coordinates (from 0 to 1), and one with the modified
        //        texture coordinates as required for cube #2.  You can either do this by modifying the cube code or by modifying
        //        a cube instance's texture_coords after it is already created.
        this.shapes = {
            box_1: new Cube(),
            box_2: new Cube(),
            axis: new Axis_Arrows()
        }

        // TODO:  Create the materials required to texture both cubes with the correct images and settings.
        //        Make each Material from the correct shader.  Phong_Shader will work initially, but when
        //        you get to requirements 6 and 7 you will need different ones.
        this.materials = {
            phong: new Material(new Textured_Phong(), {
                color: hex_color("#ffffff"),
            }),
            
            box_1_texture: new Material(new Texture_Rotate(), {
                color: hex_color("#000000"),
                ambient: 1, diffusivity: 0.5, specularity: 0.1,
                texture: new Texture("assets/stars.png", "NEAREST")
            }),
            
            box_2_texture: new Material(new Texture_Scroll_X(), {
                color: hex_color("#000000"),
                ambient: 1, diffusivity: 0.5, specularity: 0.1,
                texture: new Texture("assets/earth.gif", "LINEAR_MIPMAP_LINEAR")
            })
        }
        // set texture coords for box 2
        this.shapes.box_2.arrays.texture_coord = this.shapes.box_2.arrays.texture_coord.map(v => Vector.of(v[0]*2, v[1]*2));

        this.initial_camera_location = Mat4.look_at(vec3(0, 10, 20), vec3(0, 0, 0), vec3(0, 1, 0));

        this.rotate_boxes = 0;
        this.box_1_position = Mat4.rotation(0, 1,0,0);
        this.box_2_position = Mat4.rotation(0, 0,1,0);
        this.time = 0;

    }

    make_control_panel() {
        // TODO:  Implement requirement #5 using a key_triggered_button that responds to the 'c' key.
        this.key_triggered_button("Rotate Boxes", ["c"], () => {
            this.rotate_boxes = !this.rotate_boxes;
        });
        this.new_line();
    }

    display(context, program_state) {
        if (!context.scratchpad.controls) {
            this.children.push(context.scratchpad.controls = new defs.Movement_Controls());
            // Define the global camera and projection matrices, which are stored in program_state.
            program_state.set_camera(Mat4.translation(0, 0, -8));
        }

        program_state.projection_transform = Mat4.perspective(
            Math.PI / 4, context.width / context.height, 1, 100);

        const light_position = vec4(10, 10, 10, 1);
        program_state.lights = [new Light(light_position, color(1, 1, 1, 1), 1000)];

        let t = program_state.animation_time / 1000, dt = program_state.animation_delta_time / 1000;
        
        let box_1_model_transform = Mat4.identity();
        box_1_model_transform = box_1_model_transform.times(Mat4.translation(-2,0,0));

        let box_2_model_transform = Mat4.identity();
        box_2_model_transform = box_2_model_transform.times(Mat4.translation(2,0,0));

        if (this.rotate_boxes) {
            this.time += 0.01668; // calculated from taking each increment in time t from animation_time
            this.box_1_position = Mat4.rotation(this.time * 2, 1,0,0);
            this.box_2_position = Mat4.rotation(this.time * 3, 0,1,0)
        }
        
        box_1_model_transform = box_1_model_transform.times(this.box_1_position);
        box_2_model_transform = box_2_model_transform.times(this.box_2_position);

        this.shapes.box_1.draw(context, program_state, box_1_model_transform, this.materials.box_1_texture);
        this.shapes.box_2.draw(context, program_state, box_2_model_transform, this.materials.box_2_texture);
        
    }
}


class Texture_Scroll_X extends Textured_Phong {
    // TODO:  Modify the shader below (right now it's just the same fragment shader as Textured_Phong) for requirement #6.
    fragment_glsl_code() {
        return this.shared_glsl_code() + `
            varying vec2 f_tex_coord;
            uniform sampler2D texture;
            uniform float animation_time;
            
            void main(){
                // Sample the texture image in the correct place:

                vec2 f_tex_coord_altered = vec2(f_tex_coord.x + (mod(animation_time, 8.0)*2.0), f_tex_coord.y);
                vec4 tex_color = texture2D( texture, f_tex_coord_altered);

                if( tex_color.w < .01 ) discard;

                float u = mod(f_tex_coord.x, 1.0);
                float v = mod(f_tex_coord.y, 1.0);
                if ((u < 0.85 && u > 0.15 && v > 0.75 && v < 0.85) || 
                    (u < 0.85 && u > 0.15 && v > 0.15 && v < 0.25) || 
                    (u > 0.75 && u < 0.85 && v < 0.85 && v > 0.15) || 
                    (u > 0.15 && u < 0.25 && v < 0.85 && v > 0.15)) {
                    tex_color = vec4(0.0, 0.0, 0.0, 1.0);
                }
                
                // Compute an initial (ambient) color:
                gl_FragColor = vec4( ( tex_color.xyz + shape_color.xyz ) * ambient, shape_color.w * tex_color.w ); 

                // Compute the final color with contributions from lights:
                gl_FragColor.xyz += phong_model_lights( normalize( N ), vertex_worldspace );
        } `;
    }
}


class Texture_Rotate extends Textured_Phong {
    // TODO:  Modify the shader below (right now it's just the same fragment shader as Textured_Phong) for requirement #7.
    fragment_glsl_code() {
        return this.shared_glsl_code() + `
            varying vec2 f_tex_coord;
            uniform sampler2D texture;
            uniform float animation_time;
            void main(){

                float angle = 2.0 * 3.14 * mod(animation_time * 15.0 / 60.0, 10.0);
                mat2 rotation_matrix = mat2 (cos(angle), sin(angle), -sin(angle), cos(angle));
                
                // Sample the texture image in the correct place:
                vec4 tex_color = texture2D( texture, rotation_matrix*(f_tex_coord.xy-0.5)+0.5); 

                float u = mod(f_tex_coord.x, 1.0);
                float v = mod(f_tex_coord.y, 1.0);
                if ((u < 0.85 && u > 0.15 && v > 0.75 && v < 0.85) || 
                    (u < 0.85 && u > 0.15 && v > 0.15 && v < 0.25) || 
                    (u > 0.75 && u < 0.85 && v < 0.85 && v > 0.15) || 
                    (u > 0.15 && u < 0.25 && v < 0.85 && v > 0.15)) {
                    tex_color = vec4(0.0, 0.0, 0.0, 1.0);
                }

                if( tex_color.w < .01 ) discard;

                // Compute an initial (ambient) color:
                gl_FragColor = vec4( ( tex_color.xyz + shape_color.xyz ) * ambient, shape_color.w * tex_color.w ); 

                // Compute the final color with contributions from lights:
                gl_FragColor.xyz += phong_model_lights( normalize( N ), vertex_worldspace );
        } `;
    }
}

