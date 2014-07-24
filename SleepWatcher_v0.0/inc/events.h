typedef enum {
	BtnHold=0,
	BtnPressed,
	PowerOff
}event_t;

typedef struct{

}event;
extern uint32_t event_table;
#define setEvent(event) event_table|=1<<event
#define clearEvent(event) event_table&=~(1<<event)
#define checkEvent(event) (event_table>>event)&1
#define clearAllEvents() event_table=0
