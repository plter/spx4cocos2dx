/****************************************************************************
 Author: Luma (stubma@gmail.com)
 
 https://github.com/stubma/cocos2dx-common
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "CCUtils.h"
#include "CCMoreMacros.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    #include <sys/sysctl.h>
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	//#include "JniHelper.h"
#endif

NS_CC_BEGIN

CCUtils::StringList CCUtils::s_tmpStringList;
CCArray CCUtils::s_tmpArray;

unsigned char CCUtils::UnitScalarToByte(float x) {
    if (x < 0) {
        return 0;
    }
    if (x >= 1) {
        return 255;
    }
    return (int)(x * (1 << 16)) >> 8;
}

const char* CCUtils::copy(const char* src) {
	if(src == NULL)
		return NULL;
    
	size_t len = strlen(src);
	char* c = (char*)calloc(len + 1, sizeof(char));
	memcpy(c, src, len);
	return c;
}

const char* CCUtils::copy(const char* src, int start, size_t len) {
	if(src == NULL)
		return NULL;
    
	char* c = (char*)calloc(len + 1, sizeof(char));
	memcpy(c, src + start, len);
	return c;
}

void CCUtils::toLowercase(string& s) {
	if(s.empty())
		return;
	
    size_t len = s.length();
	char* buf = new char[len + 1];
	strcpy(buf, s.c_str());
    for(int i = 0; i < len; i++){
		if(buf[i] >= 0x41 && buf[i] <= 0x5A){
			buf[i] += 0x20;
		}
	}
	s.copy(buf, len);
	delete buf;
}

bool CCUtils::startsWith(const string& s, const string& sub) {
    return s.find(sub) == 0;
}

bool CCUtils::endsWith(const string& s, const string& sub) {
    return s.rfind(sub) == s.length() - sub.length();
}

void CCUtils::replaceChar(string& s, char c, char sub) {
    size_t len = s.length();
	char* buf = new char[len + 1];
    strcpy(buf, s.c_str());
    
	for(int i = 0; i < len; i++) {
		if(buf[i] == c) {
            buf[i] = sub;
        }
	}
    
    s.copy(buf, len);
    delete buf;
}

int CCUtils::getNumDigits(int num) {
    int d = 1;
    num /= 10;
    while(num > 0) {
        d++;
        num /= 10;
    }
    return d;
}

long CCUtils::lastDotIndex(const string& path) {
	if(path.empty())
		return -1;
    
	size_t len = path.length();
	for(int i = len - 1; i >= 0; i--) {
		if(path[i] == '.')
			return i;
	}
    
	return -1;
}

long CCUtils::lastSlashIndex(string path) {
	if(path.empty())
		return -1;
    
	// change slash to windows format
    if(CC_PATH_SEPARATOR != '/')
        replaceChar(path, '/', CC_PATH_SEPARATOR);
    
	// find slash index
	size_t len = path.length();
	int end = len;
	int slash = -1;
	for(int i = len - 1; i >= 0; i--) {
		if(path[i] == CC_PATH_SEPARATOR) {
			if(i == end - 1) {
				end--;
				if(i == 0) {
					slash = 0;
					break;
				}
			} else {
				slash = i;
				break;
			}
		}
	}
    
	// skip extra slash
	if(slash != -1) {
		while(slash >= 1 && path[slash - 1] == CC_PATH_SEPARATOR)
			slash--;
	}
    
	// assign to end
	end = slash;
	if(end == 0)
		end = 1;
    
	return end;
}

string CCUtils::lastPathComponent(const string& path) {
	// change slash to windows format
	if(CC_PATH_SEPARATOR != '/')
		replaceChar((string&)path, '/', CC_PATH_SEPARATOR);
    
	size_t len = path.length();
	int start = 0;
	int end = len;
	for(int i = len - 1; i >= 0; i--) {
		if(path[i] == CC_PATH_SEPARATOR) {
			if(i == end - 1)
				end--;
			else {
				start = i + 1;
				break;
			}
		}
	}
    
	if(end < start)
		return path;
	else
		return path.substr(start, end);
}

string CCUtils::deleteLastPathComponent(const string& path) {
	long end = lastSlashIndex(path);
	if(end < 0)
		return path;
	else
		return path.substr(0, end);
}

string CCUtils::appendPathComponent(const string& path, const string& component) {
	// change slash to windows format
	if(CC_PATH_SEPARATOR != '/')
		replaceChar((string&)path, '/', CC_PATH_SEPARATOR);
    
	// validating
	if(path.empty()) {
		if(component.empty())
			return "";
		else
			return component;
	} else if(component.empty()) {
		return path;
    }
    
	// allocate a big enough buffer
	// plus 2 because one for slash, one for null terminator
	size_t len = path.length();
	int cLen = component.length();
    char* buf = new char[len + cLen + 2];
    memset(buf, 0, len + cLen + 2);
    
	// copy path
	memcpy(buf, path.c_str(), len);
    
	// take care of slash
	int start = len;
	if(start > 0) {
		if(buf[start - 1] != CC_PATH_SEPARATOR) {
			buf[start++] = CC_PATH_SEPARATOR;
		} else {
			while(start >= 2 && buf[start - 2] == CC_PATH_SEPARATOR)
				start--;
		}
	}
    
	// copy component
	int cStart = 0;
	while(cStart < cLen && component[cStart] == CC_PATH_SEPARATOR)
		cStart++;
	if(cStart > 0 && start == 0)
		cStart--;
	memcpy(buf + start, component.c_str() + cStart, cLen - cStart);
    
    // remove end slash
    int end = start + cLen - cStart - 1;
    while(buf[end] == CC_PATH_SEPARATOR)
        buf[end--] = 0;
    
    string ret = buf;
    delete buf;
	return ret;
}

string CCUtils::deletePathExtension(const string& path) {
    long end = lastDotIndex(path);
	long slash = lastSlashIndex((string&)path);
	if(end >= 0) {
		if(end > slash)
			return path.substr(0, end);
		else
			return path;
	} else {
		return path;
	}
}

string CCUtils::getParentPath(string path) {
	if(path.empty())
		return "";
	
	size_t slash = path.rfind('/');
	if(slash == string::npos)
		return "";
	else if(slash == 0)
		return path.substr(0, 1);
	else
		return path.substr(0, slash);
}

CCPoint CCUtils::getOrigin(CCNode* node) {
	if(node->isIgnoreAnchorPointForPosition()) {
		return node->getPosition();
	} else {
		return ccp(node->getPositionX() - node->getAnchorPointInPoints().x,
				   node->getPositionY() - node->getAnchorPointInPoints().y);
	}
}

CCPoint CCUtils::getCenter(CCNode* node) {
	if(node->isIgnoreAnchorPointForPosition()) {
		return ccpAdd(node->getPosition(), ccp(node->getContentSize().width / 2, node->getContentSize().height / 2));
	} else {
		return ccpAdd(ccp(node->getPositionX() - node->getAnchorPointInPoints().x,
						  node->getPositionY() - node->getAnchorPointInPoints().y),
					  ccp(node->getContentSize().width / 2, node->getContentSize().height / 2));
	}
}

CCPoint CCUtils::getLocalCenter(CCNode* node) {
    return ccp(node->getContentSize().width / 2,
               node->getContentSize().height / 2);
}

CCPoint CCUtils::getLocalPoint(CCNode* node, float xpercent, float ypercent) {
	CCSize s = node->getContentSize();
	return ccp(s.width * xpercent, s.height * ypercent);
}

CCPoint CCUtils::getLocalPoint(CCNode* node, CCPoint anchor) {
	CCSize s = node->getContentSize();
	return ccp(s.width * anchor.x, s.height * anchor.y);
}

CCPoint CCUtils::getPoint(CCNode* node, float xpercent, float ypercent) {
	CCPoint origin = getOrigin(node);
	CCSize size = node->getContentSize();
	return ccp(origin.x + size.width * xpercent, origin.y + size.height * ypercent);
}

void CCUtils::setTreeOpacity(CCNode* n, int o) {
	// self
	CCRGBAProtocol* p = dynamic_cast<CCRGBAProtocol*>(n);
	if(p) {
		p->setOpacity((GLubyte)o);
	}
	
	// children
	CCArray* children = n->getChildren();
	CCObject* pObj = NULL;
	CCARRAY_FOREACH(children, pObj) {
		setTreeOpacity((CCNode*)pObj, o);
	}
}

CCScene* CCUtils::getScene(CCNode* n) {
    while(n) {
        CCScene* s = dynamic_cast<CCScene*>(n);
        if(s) {
            return s;
        } else {
            n = n->getParent();
        }
    }
    
    return NULL;
}

int CCUtils::binarySearch(int* a, size_t len, int key) {
	int low = 0;
	int high = len - 1;
	
	while(low <= high) {
		int mid = (low + high) >> 1;
		int midVal = a[mid];
		
		if(midVal < key)
			low = mid + 1;
		else if(midVal > key)
			high = mid - 1;
		else
			return mid; // key found
	}
	return -(low + 1); // key not found.
}

CCRect CCUtils::combine(const CCRect& r1, const CCRect& r2) {
    float left = MIN(r1.origin.x, r2.origin.x);
	float right = MAX(r1.origin.x + r1.size.width, r2.origin.x + r2.size.width);
	float bottom = MIN(r1.origin.y, r2.origin.y);
	float top = MAX(r1.origin.y + r1.size.height, r2.origin.y + r2.size.height);
	return CCRectMake(left, bottom, right - left, top - bottom);
}


int64_t CCUtils::currentTimeMillis() {
	struct timeval tv;
	gettimeofday(&tv, (struct timezone *) NULL);
	int64_t when = tv.tv_sec * 1000LL + tv.tv_usec / 1000;
	return when;
}

CCUtils::StringList& CCUtils::componentsOfString(const string& s, const char sep) {
    // remove head and tailing brace, bracket, parentheses
    int start = 0;
    int end = s.length() - 1;
    char c = s[start];
    while(c == '{' || c == '[' || c == '(') {
        start++;
        c = s[start];
    }
    c = s[end];
    while(c == '}' || c == ']' || c == ')') {
        end--;
        c = s[end];
    }
    
    // returned string list
    s_tmpStringList.clear();
    
    // iterate string
    int compStart = start;
    for(int i = start; i <= end; i++) {
        c = s[i];
        if(c == sep) {
            s_tmpStringList.push_back(s.substr(compStart, i - compStart));
            compStart = i + 1;
        } else if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            if(compStart == i) {
                compStart++;
            }
        }
    }
    
    // last comp
    if(compStart <= end) {
        s_tmpStringList.push_back(s.substr(compStart, end - compStart + 1));
    }
    
    // return
    return s_tmpStringList;
}

CCPoint CCUtils::ccpFromString(const string& s) {
    StringList comp = componentsOfString(s, ',');
    float x = 0, y = 0;
    if(comp.size() > 0) {
        x = atof(comp.at(0).c_str());
    }
    if(comp.size() > 1) {
        y = atof(comp.at(1).c_str());
    }
    return ccp(x, y);
}

CCSize CCUtils::ccsFromString(const string& s) {
    StringList comp = componentsOfString(s, ',');
    float x = 0, y = 0;
    if(comp.size() > 0) {
        x = atof(comp.at(0).c_str());
    }
    if(comp.size() > 1) {
        y = atof(comp.at(1).c_str());
    }
    return CCSizeMake(x, y);
}

CCRect CCUtils::ccrFromString(const string& s) {
    StringList comp = componentsOfString(s, ',');
    float x = 0, y = 0, w = 0, h = 0;
    if(comp.size() > 0) {
        x = atof(comp.at(0).c_str());
    }
    if(comp.size() > 1) {
        y = atof(comp.at(1).c_str());
    }
    if(comp.size() > 2) {
        w = atof(comp.at(2).c_str());
    }
    if(comp.size() > 3) {
        h = atof(comp.at(3).c_str());
    }
    return CCRectMake(x, y, w, h);
}

CCArray& CCUtils::arrayFromString(const string& s) {
    StringList comp = componentsOfString(s, ',');
    
    // clear
    s_tmpArray.removeAllObjects();
    
    // iterator components
    for(StringList::iterator iter = comp.begin(); iter != comp.end(); iter++) {
        string& cs = *iter;
        if(cs.length() > 0) {
            if(cs[0] == '\'' || cs[0] == '"') {
                int start = 1;
                int end = cs.length() - 1;
                if(cs[end] == '\'' || cs[end] == '"') {
                    end--;
                }
                if(end >= start) {
                    s_tmpArray.addObject(CCString::create(cs.substr(start, end - start + 1)));
                } else {
                    s_tmpArray.addObject(CCString::create(""));
                }
            } else {                
                float f = atof(cs.c_str());
                s_tmpArray.addObject(CCFloat::create(f));
            }
        } else {
            s_tmpArray.addObject(CCFloat::create(0));
        }
    }
    
    // return
    return s_tmpArray;
}

string CCUtils::arrayToString(const CCArray& array) {
    string ret = "[";
    CCObject* obj;
    char buf[128]; 
    CCARRAY_FOREACH(&array, obj) {
        CCString* s = dynamic_cast<CCString*>(obj);
        if(s) {
            if(ret.length() > 1)
                ret.append(",");
            ret.append("\"");
            ret.append(s->getCString());
            ret.append("\"");
            continue;
        }
        
        CCInteger* i = dynamic_cast<CCInteger*>(obj);
        if(i) {
            if(ret.length() > 1)
                ret.append(",");
            sprintf(buf, "%d", i->getValue());
            ret.append(buf);
            continue;
        }
        
        CCFloat* f = dynamic_cast<CCFloat*>(obj);
        if(f) {
            if(ret.length() > 1)
                ret.append(",");
            sprintf(buf, "%f", f->getValue());
            ret.append(buf);
            continue;
        }
        
        CCDouble* d = dynamic_cast<CCDouble*>(obj);
        if(d) {
            if(ret.length() > 1)
                ret.append(",");
            sprintf(buf, "%lf", d->getValue());
            ret.append(buf);
            continue;
        }
    }
    
    ret.append("]");
    return ret;
}

void CCUtils::setOpacityRecursively(CCNode* node, int o) {
	CCRGBAProtocol* p = dynamic_cast<CCRGBAProtocol*>(node);
	if(p) {
		p->setOpacity(o);
	}
	
	CCArray* children = node->getChildren();
    int cc = node->getChildrenCount();
    for(int i = 0; i < cc; i++) {
        CCNode* child = (CCNode*)children->objectAtIndex(i);        
        setOpacityRecursively(child, o);
    }
}

CCArray* CCUtils::getChildrenByTag(CCNode* parent, int tag) {
	CCArray* ret = CCArray::create();
	CCObject* obj;
	CCARRAY_FOREACH(parent->getChildren(), obj) {
		CCNode* child = (CCNode*)obj;
		if(child->getTag() == tag) {
			ret->addObject(child);
		}
	}
	
	return ret;
}

double CCUtils::pround(double x, int precision) {
    double div = pow(10, -precision);
    return floor(x / div + 0.5) * div;
}

double CCUtils::pfloor(double x, int precision) {
    double div = pow(10, -precision);
    return (int)floor(x / div) * div;
}

double CCUtils::pceil(double x, int precision) {
    double div = pow(10, -precision);
    return (int)ceil(x / div) * div;
}

NS_CC_END