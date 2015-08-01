//
//  EAGLView.m
//  OpenGLES12
//
//  Created by Simon Maurice on 28/04/09.
//  Copyright Simon Maurice 2009. All rights reserved.
//



#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"

#include "core_defs.h"
#include "platform.h"
#include "system.h"
#include "Game.h"

// A class extension to declare private methods
@interface EAGLView ()

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer *animationTimer;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

namespace {
    SympSystem* g_pSympSystem = NULL;
    MyGame* g_pGame = NULL;
}

@implementation EAGLView
@synthesize context;
@synthesize animationTimer;
@synthesize animationInterval;
@synthesize animating;
@dynamic animationFrameInterval;

// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}


//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {
    
    if ((self = [super initWithCoder:coder])) {
        // Get the layer
        CAEAGLLayer* eaglLayer = (CAEAGLLayer *)self.layer;
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], 
                                        kEAGLDrawablePropertyRetainedBacking, 
                                        kEAGLColorFormatRGBA8, 
                                        kEAGLDrawablePropertyColorFormat, 
                                        nil];
        
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];        
        if (!context || ![EAGLContext setCurrentContext:context]) 
		{
            [self release];
            return nil;
        }
        
        [self createFramebuffer];
        
        animationInterval = 1.0 / 60.0;
        animationFrameInterval = 1;
		newLocation[0] = 1.5;
		newLocation[1] = 1.0;
		fingerOnObject = NO;
        animating = FALSE;
        
        g_pSympSystem = new SympSystem;
        //TODO: register GAME
        g_pGame = new MyGame;
        g_pSympSystem->RegisterGameInterface( g_pGame );

        char ** c = 0;
        g_pSympSystem->Startup( 0, c, 0 );
        //Platform::pump();
    }
    
    
    
    
    
    return self;
}


- (void)drawView {
    
    [EAGLContext setCurrentContext:context];
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glViewport(0, 0, backingWidth, backingHeight);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    if ( g_pSympSystem ) {
        g_pSympSystem->Resize(backingWidth, backingHeight);
        g_pSympSystem->Update();
    }
    
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
/*
	UITouch *t = [[touches allObjects] objectAtIndex:0];
	CGPoint touchPos = [t locationInView:t.view];
	
	CGRect bounds = [self bounds];
	
    // This takes our point and makes it into a "percentage" of the screen
    //   That is 0.85 = 85%
    CGPoint p = CGPointMake((touchPos.x - bounds.origin.x) / bounds.size.width,
                            (touchPos.y - bounds.origin.y) / bounds.size.height);
    
    CGRect touchArea = CGRectMake((3.0 * p.y) - 0.1, (2.0 * p.x) - 0.1, 0.2, 0.2);
    if ((newLocation[0] > touchArea.origin.x) && (newLocation[0] < (touchArea.origin.x + touchArea.size.width))) {
        if ((newLocation[1] > touchArea.origin.y) && (newLocation[1] < (touchArea.origin.y + touchArea.size.height))) {
            fingerOnObject = YES;
        }
    }
*/
    UITouch *t = [[touches allObjects] objectAtIndex:0];
	CGPoint touchPos = [t locationInView:t.view];
     /*
    NSLog(@"START %f %f", touchPos.x, touchPos.y );
    */
    cb_mouseclick( MOUSE_BUTTON_LEFT, 1, (int)touchPos.x, backingHeight - (int)touchPos.y);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	/*
	if (!fingerOnObject) {
		return;
	}
	
    UITouch *t = [[touches allObjects] objectAtIndex:0];
    CGPoint touchPos = [t locationInView:t.view];
    
    CGRect bounds = [self bounds];
    
    // This takes our point and makes it into a "percentage" of the screen
    //   That is 0.85 = 85%
    CGPoint p = CGPointMake((touchPos.x - bounds.origin.x) / bounds.size.width,
                            (touchPos.y - bounds.origin.y) / bounds.size.height);
    
    newLocation[0] = 3.0 * p.y;
    newLocation[1] = 2.0 * p.x;
     */
    
    UITouch *t = [[touches allObjects] objectAtIndex:0];
    CGPoint touchPos = [t locationInView:t.view];
    newLocation[0] = touchPos.x;
    newLocation[1] = touchPos.y;
    

    
    //NSLog(@"%f %f", touchPos.x, touchPos.y );
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    /*
	fingerOnObject = NO;
     */
    //NSLog(@"STOP");
    cb_mouseclick( MOUSE_BUTTON_LEFT, 0, (int)newLocation[0], backingHeight - (int)newLocation[1] );
}


- (void)layoutSubviews {
    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
    [self drawView];
}


- (BOOL)createFramebuffer {
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    CHECK_GL_ERRORS();
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);

    glGenFramebuffersOES(1, &viewFramebuffer);
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    CHECK_GL_ERRORS();
    
    return YES;
}


- (void)destroyFramebuffer {
    
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    
    if(depthRenderbuffer) {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}


- (NSInteger) animationFrameInterval
{
	return animationFrameInterval;
}

- (void) setAnimationFrameInterval:(NSInteger)frameInterval
{
	// Frame interval defines how many display frames must pass between each time the
	// display link fires. The display link will only fire 30 times a second when the
	// frame internal is two on a display that refreshes 60 times a second. The default
	// frame interval setting of one will fire 60 times a second when the display refreshes
	// at 60 times a second. A frame interval setting of less than one results in undefined
	// behavior.
	if (frameInterval >= 1)
	{
		animationFrameInterval = frameInterval;
		
		if (animating)
		{
			[self stopAnimation];
			[self startAnimation];
		}
	}
}

- (void) startAnimation
{
	if (!animating)
	{
        m_displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView)];
        [m_displayLink setFrameInterval:animationFrameInterval];
        [m_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		animating = TRUE;
	}
}

- (void)stopAnimation
{
	if (animating)
	{
        [m_displayLink invalidate];
        m_displayLink = nil;
		animating = FALSE;
	}
}


- (void)dealloc {
    
    [self stopAnimation];
    
    delete g_pSympSystem;
    delete g_pGame;
    
    if ([EAGLContext currentContext] == context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [context release];  
    [super dealloc];
}

@end
