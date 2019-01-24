#ifndef __ORDERRECORD_H__
#define __ORDERRECORD_H__

#include <map>
#include <vector>

#include <cl-util/factory.h>
#include <cl-util/debug_stream.h>
#include <clite/message.h>

#include "DataManager.h"
#include "Markets.h"
#include "Suggestions.h"

using std::map;
using std::vector;
using namespace clite::util;

/**
 * Holds some execution-specific info about orders we placed
 * 
 * Notes:
 * - Member variables (except for _cancelReason) were originally declared const.
 * - However, this causes trouble when trying to declare templated STL containers
 *   that hold OrderRecords (because STL containers need an assignment operator, and
 *   because the assignment operator cant overwrite const member variables).
 * - We could try to define an assignment operator that does not overwrite const
 *   member variables, and then try to use the STL containers in a way that does not
 *   cause the assignment operator to be called - but this is a brittle solution.
 * - Instead we choose to make the member variables non-const. 
 */
class OrderRecord {

 public:

  //////////////
  // Id numbers
  //////////////
  int _orderId;
  int _tradeLogicId;    /// An Id specifying which (instance of a) Trade Logic decided to place the order.
  int _componentId;     /// An Id specifying which (instance of a) TradeLogicComponent decided to place the order.
  int _componentSeqNum; /// A sequence-number generated by the component for each placement-suggestion it makes

  ///////////////////
  // Plcment/Cxl reasons
  ///////////////////
  OrderPlacementSuggestion::PlacementReason _placementReason;
  OrderCancelSuggestion::CancelReason             _cancelReason; /// not set on constructor

  //////////////
  // Some fields copied from the Order class
  //////////////
  int       _cid;
  Mkt::Side _side;
  int       _size;
  double    _price;
  double    _mid;
  double    _vol;
  ECN::ECN  _ecn;
  TimeVal   _placingTV; // The time we sent the placement-order

  //////////////
  // Used for TC analysis
  //////////////  
  double _priority;            // Leaf-node OPC priority, not original request priority.
  double _marketImpact;        // Estimated market-impact for entire order, at time of placement.

  /////////////////
  // Q-position info
  /////////////////
  TimeVal   _placeDuTV;        // Add tv (time we got the placement-data-update that matches this order)
  TimeVal   _confirmationTV; // the tv of the confirmation-order-update

  uint64_t  _refnum;     // The reference number from the exchange (either the value we got from the exchange or a guessed value)
  bool      _refGuessed; // Did we have to guess ref-num or simply got it back from the exchange

  int       _initialShsPos;    // initial #shs in front of us
  int       _nShsReducedFront; // # of pulled/filled shares ahead of you in queue, since you showed up in queue (running total).

 public:
  int       nShsInFront() const { return _initialShsPos - _nShsReducedFront; }

  //////////////
  // Constructor
  //////////////
  OrderRecord( const Order* order, const OrderPlacementSuggestion& placementMsg);
  
  void setCancelReason( OrderCancelSuggestion::CancelReason cancelReason ) { _cancelReason = cancelReason; }
  int orderId() const { return _orderId; }
  int tradeLogicId() const { return _tradeLogicId;}
  int componentId() const { return _componentId;}
  int componentSeqNum() const { return _componentSeqNum;}
  OrderPlacementSuggestion::PlacementReason placementReason() const { return _placementReason;}
  OrderCancelSuggestion::CancelReason cancelReason() const { return _cancelReason;}

  int snprint(char *buf, int n) const; 

  ///////////////////
  // Update functions
  ///////////////////
  
};

#endif  // __ORDERRECORD_H__