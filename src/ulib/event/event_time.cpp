// ============================================================================
//
// = LIBRARY
//    ULib - c++ library
//
// = FILENAME
//   event_time.cpp
//
// = AUTHOR
//    Stefano Casazza
//
// ============================================================================

#include <ulib/event/event_time.h>

#ifdef USE_LIBEVENT
#  include <ulib/libevent/event.h>
#endif

UEventTime::UEventTime(long sec, long usec) : UTimeVal(sec, usec)
{
   U_TRACE_REGISTER_OBJECT(0, UEventTime, "%ld,%ld", sec, usec)

   reset();

#ifdef USE_LIBEVENT
   if (u_ev_base == 0) u_ev_base = (struct event_base*) U_SYSCALL_NO_PARAM(event_init);

   U_INTERNAL_ASSERT_POINTER(u_ev_base)

   pevent = U_NEW(UTimerEv<UEventTime>(*this));

   (void) UDispatcher::add(*pevent, *(UTimeVal*)this);
#endif
}

UEventTime::~UEventTime()
{
   U_TRACE_UNREGISTER_OBJECT(0, UEventTime)

#ifdef USE_LIBEVENT
   UDispatcher::del(pevent);
             delete pevent;
#endif
}

#ifdef USE_LIBEVENT
void UEventTime::operator()(int fd, short event)
{
   U_TRACE(0, "UEventTime::operator()(%d,%hd)", fd, event)

   int result = handlerTime();

   // return value:
   // ---------------
   // -1 - normal
   //  0 - monitoring
   // ---------------

   if (result == 0) (void) UDispatcher::add(*pevent, *(UTimeVal*)this);
}
#endif

void UEventTime::setCurrentTime()
{
   U_TRACE(1, "UEventTime::setCurrentTime()")

   U_CHECK_MEMORY

   (void) U_SYSCALL(gettimeofday, "%p,%p", u_now, 0);

   U_INTERNAL_DUMP("u_now = { %ld %6ld }", u_now->tv_sec, u_now->tv_usec)

   ctime = *u_now;
}

void UEventTime::setTimerVal(struct timeval* it_value)
{
   U_TRACE(0, "UEventTime::setTimerVal(%p)", it_value)

   U_CHECK_MEMORY

   it_value->tv_sec  = ctime.tv_sec  + tv_sec  - u_now->tv_sec;
   it_value->tv_usec = ctime.tv_usec + tv_usec - u_now->tv_usec;

   UTimeVal::adjust(&(it_value->tv_sec), &(it_value->tv_usec));

   U_INTERNAL_DUMP("it_value = { %ld %6ld }", it_value->tv_sec, it_value->tv_usec)

   U_INTERNAL_ASSERT(it_value->tv_sec  >= 0)
   U_INTERNAL_ASSERT(it_value->tv_usec >= 0)
}

__pure bool UEventTime::isOld() const
{
   U_TRACE(0, "UEventTime::isOld()")

   U_CHECK_MEMORY

   long t1 = (ctime.tv_sec + tv_sec);

   U_INTERNAL_DUMP("this = { %ld %6ld }", t1, ctime.tv_usec + tv_usec)

   bool result = (  t1  < u_now->tv_sec) ||
                  ((t1 == u_now->tv_sec) &&
                   ((ctime.tv_usec + tv_usec) < u_now->tv_usec));

   U_RETURN(result);
}

__pure bool UEventTime::isExpired() const
{
   U_TRACE(0, "UEventTime::isExpired()")

   U_CHECK_MEMORY

   U_INTERNAL_DUMP("this = { %ld %6ld }", ctime.tv_sec + tv_sec, ctime.tv_usec + tv_usec)

   long diff  = (ctime.tv_sec  + tv_sec  - u_now->tv_sec)  * 1000L +
               ((ctime.tv_usec + tv_usec - u_now->tv_usec) / 1000L);

   U_DUMP("diff = %ld", diff)

   if (diff <= 0) U_RETURN(true);

   long delta = UTimeVal::getMilliSecond() / 128;

   U_DUMP("delta = %ld", diff, delta)

   if (diff <= delta) U_RETURN(true);

   U_RETURN(false);
}

__pure bool UEventTime::operator<(const UEventTime& t) const
{
   U_TRACE(0, "UEventTime::operator<(%p)", &t)

   long t1 = (  ctime.tv_sec +   tv_sec),
        t2 = (t.ctime.tv_sec + t.tv_sec);

   U_INTERNAL_DUMP("{ %ld %6ld } < { %ld %6ld }", t1,   ctime.tv_usec +   tv_usec,
                                                  t2, t.ctime.tv_usec + t.tv_usec)

   bool result = (  t1 <  t2) ||
                  ((t1 == t2) &&
                   ((ctime.tv_usec + tv_usec) < (t.ctime.tv_usec + t.tv_usec)));

   U_INTERNAL_DUMP("result = %b", result)

   return result;
}

// STREAM

#ifdef U_STDCPP_ENABLE
U_EXPORT ostream& operator<<(ostream& os, const UEventTime& t)
{
   U_TRACE(0, "UEventTime::operator<<(%p,%p)", &os, &t)

   os.put('{');
   os.put(' ');
   os << t.ctime.tv_sec;
   os.put(' ');
   os.width(6);
   os << t.ctime.tv_usec;
   os.put(' ');
   os.put('{');
   os.put(' ');
   os << t.tv_sec;
   os.put(' ');
   os.width(6);
   os << t.tv_usec;
   os.put(' ');
   os.put('}');
   os.put(' ');
   os.put('}');

   return os;
}

// DEBUG

#  ifdef DEBUG
const char* UEventTime::dump(bool _reset) const
{
   UTimeVal::dump(false);

   *UObjectIO::os << '\n'
                  << "ctime   " << "{ " << ctime.tv_sec
                                << " "  << ctime.tv_usec
                                << " }";

   if (_reset)
      {
      UObjectIO::output();

      return UObjectIO::buffer_output;
      }

   return 0;
}
#  endif
#endif
