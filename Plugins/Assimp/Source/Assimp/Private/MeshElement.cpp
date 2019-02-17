#include "MeshElement.h"


UIObject::UIObject() :mSelected(false)
{

}

UIObject::~UIObject()
{

}

void UIObject::Select(bool select)
{
	mSelected = select;
}

void UIObject::Hover(bool hovered)
{
	mHovered = hovered;
}

void UIObject::Destroy()
{
	ConditionalBeginDestroy();
}

bool UIObject::IsElementSelected()
{
	return mSelected;
}

bool UIObject::IsElementHovered()
{
	return mHovered;
}

UIElement::UIElement()
{

}

UIElement::~UIElement()
{

}

FORCEINLINE FVector UIElement::GetPosition()
{
	return FVector::ZeroVector;
}

void UIElement::SetPosition(FVector Position)
{
	OnPositionSet.Broadcast(Position);
}

void UIElement::OnGrabbed()
{

}

void UIElement::OnReleased()
{

}

void UIElement::OnTriggerPressed()
{
	Select(!IsElementSelected());
}

void UIElement::OnTriggerReleased()
{
	
}


void UIElement::Destroy()
{
	if(mGrabbableBoxComponent)
		mGrabbableBoxComponent->DestroyComponent();

	UIObject::Destroy();
}

void UIElement::SetGrabbableBoxComponent(UActorComponent* GrabbableBoxComponent)
{
	mGrabbableBoxComponent = GrabbableBoxComponent;
}
