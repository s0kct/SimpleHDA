#import <Foundation/Foundation.h>
int main(int argc, const char **argv) { @autoreleasepool {
NSBundle *b=[NSBundle bundleWithPath:[NSString stringWithUTF8String:argv[1]]];
if (!b || b.localizations.count != 41) return 1;
for (NSString *lang in b.localizations) {
 NSBundle *l=[NSBundle bundleWithPath:[b pathForResource:lang ofType:@"lproj"]];
 NSString *s=[l localizedStringForKey:@"DeviceName" value:nil table:nil];
 if (!s.length || [s isEqualToString:@"DeviceName"]) return 2;
}
printf("PASS: NSBundle resolves DeviceName in all 41 catalogs\n");
} return 0; }
