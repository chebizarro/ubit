#pragma once

#include <ubit/ubox.hpp>
#include "gmock/gmock.h"


namespace ubit {

class MockUBox : public UBox {
 public:
  MOCK_METHOD0(toBox,
      UBox*());
  MOCK_CONST_METHOD0(toBox,
      const UBox*());
  MOCK_CONST_METHOD0(getDisplayType,
      int());
  MOCK_METHOD2(update,
      void(const UUpdate, UDisp*));
  MOCK_METHOD2(doUpdate,
      void(const UUpdate, UDisp*));
  MOCK_CONST_METHOD0(getViewCount,
      int());
  MOCK_CONST_METHOD1(getViews,
      int(std::vector<UView*>&));
  MOCK_CONST_METHOD1(getView,
      UView*(int));
  MOCK_CONST_METHOD1(getView,
      UView*(const UInputEvent&));
  MOCK_CONST_METHOD3(getView,
      UView*(UWin&, UPoint, UDisp*));
  MOCK_CONST_METHOD1(getViewContaining,
      UView*(const UView& child_view));
  MOCK_CONST_METHOD1(getFirstViewInside,
      UView*(const UView& parent_view));
  MOCK_CONST_METHOD1(retrieveRelatedViews,
      int(std::vector<UView*>& collating_vector));
  MOCK_METHOD1(getViewInImpl,
      UView*(UView* parent_view));
  MOCK_METHOD2(getViewInImpl,
      UView*(UView* parent_view, UDimension&));
  MOCK_METHOD1(addViewImpl,
      void(UView*));
  MOCK_METHOD1(initView,
      void(UView* parent_view));
  MOCK_METHOD1(initChildViews,
      void(UElem*));
  MOCK_METHOD1(deleteViewsInside,
      void(const std::vector<UView*>& parent_views));
};

}  // namespace ubit
