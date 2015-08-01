//
//  EAGLView.h
//  OpenGLES12

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>


#ifndef DISTRIBUTION

#define CHECK_GL_ERRORS() \
do {                                                                                            	\
GLenum error = glGetError();                                                                	\
if(error != GL_NO_ERROR) {                                                                   	\
NSLog(@"OpenGL: %s [error %d]", __FUNCTION__, (int)error);					\
assert(0); \
} \
} while(false)

#else

#define CHECK_GL_ERRORS()

#endif

/*
This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
The view content is basically an EAGL surface you render your OpenGL scene into.
Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
*/
@interface EAGLView : UIView {
    
@private
    /* The pixel dimensions of the backbuffer */
    GLint backingWidth;
    GLint backingHeight;
    
    EAGLContext *context;
    
    /* OpenGL names for the renderbuffer and framebuffers used to render to this view */
    GLuint viewRenderbuffer, viewFramebuffer;
    
    /* OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist) */
    GLuint depthRenderbuffer;
    
    NSTimer *animationTimer;
    NSTimeInterval animationInterval;
    
    id m_displayLink;
    NSInteger animationFrameInterval;
    BOOL animating;
	
	GLfloat newLocation[2];
	BOOL fingerOnObject;
}

@property NSTimeInterval animationInterval;
@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void)startAnimation;
- (void)stopAnimation;
- (void)drawView;

@end
