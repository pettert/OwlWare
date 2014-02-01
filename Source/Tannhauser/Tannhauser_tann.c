/*
 * DO NOT MODIFY. THIS CODE IS MACHINE GENERATED BY THE TANNHÄUSER PD COMPILER.
 */

/*
 * System Includes
 */

#include <string.h>
#include <stdarg.h>
#include "Tannhauser_tann.h"



/*
 * Function Declarations
 */

static void mRecv_nXF4e_onMessage(TannBase *, int, PdMessage *);
static void mRecv_4jxjT_onMessage(TannBase *, int, PdMessage *);
static void mRecv_iIF6Z_onMessage(TannBase *, int, PdMessage *);
static void mAdd_DUfyp_sendMessage(TannBase *, int, PdMessage *);
static void mMultiply_2cB96_sendMessage(TannBase *, int, PdMessage *);
static void mAdd_n5QY1_sendMessage(TannBase *, int, PdMessage *);
static void mMultiply_vyKkT_sendMessage(TannBase *, int, PdMessage *);
static void mRecv_9a4RK_onMessage(TannBase *, int, PdMessage *);



/*
 * Static Helper Functions
 */

static void ctx_intern_scheduleMessageForReceiver(TannBase *const _c, const char *name, PdMessage *m) {
    // the message must occur in the future, at least after the start of the current block
	msg_setTimestamp(m, fmax(msg_getTimestamp(m), _c->blockStartTimestampMs));
	
	if (!strncmp(name, "Channel-C", 9)) {
		ctx_scheduleMessage(Base(_c), m, &mRecv_4jxjT_onMessage, 0);
		return;
	}
	if (!strncmp(name, "Channel-B", 9)) {
		ctx_scheduleMessage(Base(_c), m, &mRecv_nXF4e_onMessage, 0);
		return;
	}
	if (!strncmp(name, "Channel-A", 9)) {
		ctx_scheduleMessage(Base(_c), m, &mRecv_9a4RK_onMessage, 0);
		return;
	}
	if (!strncmp(name, "Channel-D", 9)) {
		ctx_scheduleMessage(Base(_c), m, &mRecv_iIF6Z_onMessage, 0);
		return;
	}
}

static struct MessageTable *ctx_intern_getTableForName(TannBase *const _c, const char *tableName) {
	
	return NULL; // by default, return NULL
}



/*
 * Context Include and Implementatons
 */

Tann_tann *ctx_tann_new(int numInputChannels, int numOutputChannels, int blockSize, double sampleRate) {
	Tann_tann *const _c = (Tann_tann *) malloc(sizeof(Tann_tann));

	Base(_c)->numInputChannels = numInputChannels;
	Base(_c)->numOutputChannels = numOutputChannels;
	Base(_c)->blockSize = blockSize;
	Base(_c)->sampleRate = sampleRate;
	Base(_c)->blockStartTimestampMs = 0.0;
	Base(_c)->f_scheduleMessageForReceiver = &ctx_intern_scheduleMessageForReceiver;
	Base(_c)->f_getTableForName = &ctx_intern_getTableForName;
	mq_init(&Base(_c)->mq);
	Base(_c)->basePath = NULL;
	Base(_c)->printHook = NULL;
	Base(_c)->sendHook = NULL;
	Base(_c)->userData = NULL;

	dOsc_init(&_c->dOsc_yd3Kz, 880.0f, ctx_getSampleRate(Base(_c)));
	dOsc_init(&_c->dOsc_lFWZE, 2.0f, ctx_getSampleRate(Base(_c)));
	mAdd_init(&_c->mAdd_DUfyp, 0.1f);
	mMultiply_init(&_c->mMultiply_2cB96, 1000.0f);
	mAdd_init(&_c->mAdd_n5QY1, 100.0f);
	mMultiply_init(&_c->mMultiply_vyKkT, 20.0f);

	return _c;
}

void ctx_tann_free(Tann_tann *_c) {
	dOsc_free(&_c->dOsc_yd3Kz);
	dOsc_free(&_c->dOsc_lFWZE);

	free(Base(_c)->basePath);
	mq_free(&Base(_c)->mq); // free queue after all objects have been freed, messages may be cancelled

	free(_c);
}



/*
 * Static Function Implementation
 */

static void mRecv_nXF4e_onMessage(TannBase *_c, int inletIndex, PdMessage *m) {
	mMultiply_onMessage(_c, &Context(_c)->mMultiply_vyKkT, 0, m, &mMultiply_vyKkT_sendMessage);
}

static void mRecv_4jxjT_onMessage(TannBase *_c, int inletIndex, PdMessage *m) {
	
}

static void mRecv_iIF6Z_onMessage(TannBase *_c, int inletIndex, PdMessage *m) {
	
}

static void mAdd_DUfyp_sendMessage(TannBase *_c, int letIndex, PdMessage *m) {
	dOsc_onMessage(&Context(_c)->dOsc_lFWZE, 0, m);
}

static void mMultiply_2cB96_sendMessage(TannBase *_c, int letIndex, PdMessage *m) {
    mAdd_onMessage(_c, &Context(_c)->mAdd_n5QY1, 0, m, &mAdd_n5QY1_sendMessage);
}

static void mAdd_n5QY1_sendMessage(TannBase *_c, int letIndex, PdMessage *m) {
	dOsc_onMessage(&Context(_c)->dOsc_yd3Kz, 0, m);
}

static void mMultiply_vyKkT_sendMessage(TannBase *_c, int letIndex, PdMessage *m) {
    mAdd_onMessage(_c, &Context(_c)->mAdd_DUfyp, 0, m, &mAdd_DUfyp_sendMessage);
}

static void mRecv_9a4RK_onMessage(TannBase *_c, int inletIndex, PdMessage *m) {
	mMultiply_onMessage(_c, &Context(_c)->mMultiply_2cB96, 0, m, &mMultiply_2cB96_sendMessage);
}



/*
 * Context Process Implementation
 */

void ctx_tann_process(Tann_tann *const _c, float *const inputBuffers, float *const outputBuffers, const int n) {

	// process all of the messages for this block
	const double nextBlockStartTimestampMs = Base(_c)->blockStartTimestampMs + (1000.0*n/ctx_getSampleRate(Base(_c)));
	while (mq_hasMessageBefore(&Base(_c)->mq, nextBlockStartTimestampMs)) {
		MessageNode *const node = mq_peek(&Base(_c)->mq);
		node->sendMessage(Base(_c), node->let, node->m);
		mq_pop(&Base(_c)->mq);
	}

	// define a macro to refer to input buffers
	#define I(_x) (inputBuffers+((_x)*n))

	// define a macro to refer to output buffers
	#define O(_x) (outputBuffers+((_x)*n))
	utils_clearBuffer(outputBuffers, ctx_getNumOutputChannels(Base(_c))*n); // clear the output buffer

	// define the zero buffer
	float ZERO_BUFFER[n];
	utils_clearBuffer(ZERO_BUFFER, n);

	// initialise temporary signal buffers
	#define B(_x) (buffer+((_x)*n))
	float buffer[2 * n];

	// process all signal functions
	dOsc_processM(Base(_c), &_c->dOsc_yd3Kz, B(0), ctx_getSampleRate(Base(_c)), n); // osc~ 880
	dOsc_processM(Base(_c), &_c->dOsc_lFWZE, B(1), ctx_getSampleRate(Base(_c)), n); // osc~ 2
	dMult_processSS(B(0), B(1), B(1), n); // *~
	dMult_processSK(B(1), 0.2f, B(1), n); // *~ 0.2
	dAdd_processSS(B(1), O(0), O(0), n); // dac~(0)
	dAdd_processSS(B(1), O(1), O(1), n); // dac~(1)

	// now that the block has been processed, update the block timestamp
	Base(_c)->blockStartTimestampMs = nextBlockStartTimestampMs;
}