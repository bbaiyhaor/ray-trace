#include "kdnode.h"
#include "global.h"
#define DEBUG
#ifdef DEBUG
#include "debug.h"
#endif
typedef Scene::Triangle Triangle;
const int MAXN = 500000;
int axis_idl[MAXN];
int axis_idr[MAXN];
KdNode* KdNode::build(vector<Triangle*>& tris, vector<AabbBox*>& trib, int depth)
{
	KdNode* node = new KdNode();
	node->left = 0;
	node->right = 0;
	double size = tris.size();
	//	figure the box
	double x[2] = {INF, -INF}, y[2] = {INF, -INF}, z[2] = {INF, -INF};
	for (int i = 0; i < size; i++){
		x[0] = min(x[0], trib[i]->getMin().x);
		x[1] = max(x[1], trib[i]->getMax().x);
		y[0] = min(y[0], trib[i]->getMin().y);
		y[1] = max(y[1], trib[i]->getMax().y);
		z[0] = min(z[0], trib[i]->getMin().z);
		z[1] = max(z[1], trib[i]->getMax().z);
	}
	node->contained = AabbBox(Vec3(x[0], y[0], z[0]), Vec3(x[1], y[1], z[1]));
	if (size < 30){
		for (int i = 0; i < size; i++)		
			node->triangles.push_back(tris[i]);		
		return node;
	}
	int long_axis;
	if (node->contained.getSize().x > node->contained.getSize().y
		&& node->contained.getSize().x > node->contained.getSize().z)
		long_axis = 0;
	else if (node->contained.getSize().y > node->contained.getSize().z)
		long_axis = 1;
	else
		long_axis = 2;
	
	//	sort according to long axis
	for (int i = 0; i < size; i++)
		axis_idl[i] = axis_idr[i] = i;
	sort(axis_idl, axis_idl + (int)size, [&trib, &long_axis](int lhs, int rhs){
		return trib[lhs]->getMin().elem[long_axis] < trib[rhs]->getMin().elem[long_axis];
	});
	sort(axis_idr, axis_idr + (int)size, [&trib, &long_axis](int lhs, int rhs){
		return trib[lhs]->getMax().elem[long_axis] < trib[rhs]->getMax().elem[long_axis];
	});
	vector<Triangle*> lt, rt;
	vector<AabbBox*> lb, rb;	
	//	div into two groups of which the number is almost equal to each other
	double mid = trib[axis_idl[0]]->getMin().elem[long_axis];
	int ltot = 0, rtot = size, pr = 0;
	for (int i = 1; i < size; i++){
		mid = trib[axis_idl[i]]->getMin().elem[long_axis];
		if (trib[axis_idl[i - 1]]->getMin().elem[long_axis] < mid)
			ltot++;
		for (; trib[axis_idr[pr]]->getMax().elem[long_axis] < mid; pr++)
			rtot--;
		if (rtot - ltot < 3)
			break;
	}
	double midsize = size / 2 + 0.5;
	int total = 0;
	double xx[2][2] = {INF, -INF, INF, -INF}, yy[2][2] = {INF, -INF, INF, -INF}, zz[2][2] = {INF, -INF, INF, -INF};
	for (int i = 0; i < size; i++){
		int flag = 0;
		if (trib[i]->getMin().elem[long_axis] < mid){
			lt.push_back(tris[i]);
			lb.push_back(trib[i]);
			xx[0][0] = min(xx[0][0], trib[i]->getMin().x);
			xx[0][1] = max(xx[0][1], trib[i]->getMax().x);
			yy[0][0] = min(yy[0][0], trib[i]->getMin().y);
			yy[0][1] = max(yy[0][1], trib[i]->getMax().y);
			zz[0][0] = min(zz[0][0], trib[i]->getMin().z);
			zz[0][1] = max(zz[0][1], trib[i]->getMax().z);
			flag--;
		}
		if (trib[i]->getMax().elem[long_axis] > mid){
			rt.push_back(tris[i]);
			rb.push_back(trib[i]);
			xx[1][0] = min(xx[1][0], trib[i]->getMin().x);
			xx[1][1] = max(xx[1][1], trib[i]->getMax().x);
			yy[1][0] = min(yy[1][0], trib[i]->getMin().y);
			yy[1][1] = max(yy[1][1], trib[i]->getMax().y);
			zz[1][0] = min(zz[1][0], trib[i]->getMin().z);
			zz[1][1] = max(zz[1][1], trib[i]->getMax().z);
			flag++;
		}
		if (! flag)
			total++;		
	}
	//	if the number of triangles which are in the both boxes is over mid size
	//	then stop subdiv, it's a leaf
	if (total < midsize){
		node->left = new KdNode();
		node->left->contained = AabbBox(Vec3(xx[0][0], yy[0][0], zz[0][0]), Vec3(xx[0][1], yy[0][1], zz[0][1]));
		node->left->contained.setMax().elem[long_axis] = mid;
		subbuild(node->left, lt, lb, depth + 1);

		node->right = new KdNode();		
		node->right->contained = AabbBox(Vec3(xx[1][0], yy[1][0], zz[1][0]), Vec3(xx[1][1], yy[1][1], zz[1][1]));
		node->right->contained.setMin().elem[long_axis] = mid;
		subbuild(node->right, rt, rb, depth + 1);
	}
	else{
		for (int i = 0; i < size; i++)	
			node->triangles.push_back(tris[i]);
	}
	return node;
}
void KdNode::subbuild(KdNode* node, vector<Triangle*>& tris, vector<AabbBox*>& trib, int depth)
{
	node->left = 0;
	node->right = 0;
	double size = tris.size();
	//	figure the box
	if (size < 30){
		for (int i = 0; i < size; i++)		
			node->triangles.push_back(tris[i]);		
		return;
	}
	int long_axis;
	if (node->contained.getSize().x > node->contained.getSize().y
		&& node->contained.getSize().x > node->contained.getSize().z)
		long_axis = 0;
	else if (node->contained.getSize().y > node->contained.getSize().z)
		long_axis = 1;
	else
		long_axis = 2;
	
	//	sort according to long axis
	for (int i = 0; i < size; i++)
		axis_idl[i] = axis_idr[i] = i;
	sort(axis_idl, axis_idl + (int)size, [&trib, &long_axis](int lhs, int rhs){
		return trib[lhs]->getMin().elem[long_axis] < trib[rhs]->getMin().elem[long_axis];
	});
	sort(axis_idr, axis_idr + (int)size, [&trib, &long_axis](int lhs, int rhs){
		return trib[lhs]->getMax().elem[long_axis] < trib[rhs]->getMax().elem[long_axis];
	});
	vector<Triangle*> lt, rt;
	vector<AabbBox*> lb, rb;	

	double mid = trib[axis_idl[0]]->getMin().elem[long_axis];
	int ltot = 0, rtot = size, pr = 0;
	for (int i = 1; i < size; i++){
		mid = trib[axis_idl[i]]->getMin().elem[long_axis];
		if (trib[axis_idl[i - 1]]->getMin().elem[long_axis] < mid)
			ltot++;
		for (; trib[axis_idr[pr]]->getMax().elem[long_axis] < mid; pr++)
			rtot--;
		if (rtot - ltot < 5)
			break;
	}
	double midsize = size / 2 + 0.5;
	int total = 0;
	double xx[2][2] = {INF, -INF, INF, -INF}, yy[2][2] = {INF, -INF, INF, -INF}, zz[2][2] = {INF, -INF, INF, -INF};
	for (int i = 0; i < size; i++){
		int flag = 0;
		if (trib[i]->getMin().elem[long_axis] < mid){
			lt.push_back(tris[i]);
			lb.push_back(trib[i]);
			xx[0][0] = min(xx[0][0], trib[i]->getMin().x);
			xx[0][1] = max(xx[0][1], trib[i]->getMax().x);
			yy[0][0] = min(yy[0][0], trib[i]->getMin().y);
			yy[0][1] = max(yy[0][1], trib[i]->getMax().y);
			zz[0][0] = min(zz[0][0], trib[i]->getMin().z);
			zz[0][1] = max(zz[0][1], trib[i]->getMax().z);
			flag--;
		}
		if (trib[i]->getMax().elem[long_axis] > mid){
			rt.push_back(tris[i]);
			rb.push_back(trib[i]);
			xx[1][0] = min(xx[1][0], trib[i]->getMin().x);
			xx[1][1] = max(xx[1][1], trib[i]->getMax().x);
			yy[1][0] = min(yy[1][0], trib[i]->getMin().y);
			yy[1][1] = max(yy[1][1], trib[i]->getMax().y);
			zz[1][0] = min(zz[1][0], trib[i]->getMin().z);
			zz[1][1] = max(zz[1][1], trib[i]->getMax().z);
			flag++;
		}
		if (! flag)
			total++;		
	}
	if (total < midsize){
		node->left = new KdNode();
		node->left->contained = AabbBox(Vec3(xx[0][0], yy[0][0], zz[0][0]), Vec3(xx[0][1], yy[0][1], zz[0][1]));
		node->left->contained.setMax().elem[long_axis] = mid;
		subbuild(node->left, lt, lb, depth + 1);
		
		node->right = new KdNode();
		node->right->contained = AabbBox(Vec3(xx[1][0], yy[1][0], zz[1][0]), Vec3(xx[1][1], yy[1][1], zz[1][1]));
		node->right->contained.setMin().elem[long_axis] = mid;
		subbuild(node->right, rt, rb, depth + 1);
	}
	else{
		for (int i = 0; i < size; i++)	
			node->triangles.push_back(tris[i]);
	}
}

int KdNode::hit(KdNode* node, const Ray& ray, IntersectResult& result, int depth)
{
	//	never intersect
	IntersectResult judge;	
	if (!node)
		return 0;
	if ((! node->contained.contain(ray.o)) && (! node->contained.intersect(ray, judge)))
		return 0;
	
	double size = node->triangles.size();
	//leave node
	if (size > 0){
		int re = 0;
		for (int i = 0; i < size; i++){
			IntersectResult temp;	
			int tempv = node->triangles[i]->intersect(ray, temp);
			//	the intersection should be in node->contained
			//	or regard intersection did not happen
			if (tempv && node->contained.contain(temp.inter) && temp.dist < result.dist){
				re = tempv;
				result = temp;			
			}
		}
		return re;
	}
	//	ray.o in left
	if (node->left->contained.contain(ray.o)){
		int re = hit(node->left, ray, result, depth + 1);
		//	hit in left, so it's the shortest
		if (re)
			return re;
		return hit(node->right, ray, result, depth + 1);
	}
	else if (node->right->contained.contain(ray.o)){
		int re = hit(node->right, ray, result, depth + 1);
		if (re)
			return re;
		return hit(node->left, ray, result, depth + 1);
	}
	//	ray.o outside both
	else {
		IntersectResult templ, tempr;
		int rel = node->left->contained.intersect(ray, templ);
		int rer = node->right->contained.intersect(ray, tempr);
		if (! rer)
			return hit(node->left, ray, result, depth + 1);
		else if (!rel)
			return hit(node->right, ray, result, depth + 1);
		else if (templ.dist < tempr.dist){
			int re = hit(node->left, ray, result, depth + 1);
			if (re)
				return re;
			return hit(node->right, ray, result, depth + 1);
		}
		else{
			int re = hit(node->right, ray, result, depth + 1);
			if (re)
				return re;
			return hit(node->left, ray, result, depth + 1);
		}
	}
}
