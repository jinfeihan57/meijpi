// 注释
/*
    注释
*/

// 布尔值
let xxFlag: boolean = false;

// 数字
let xxCount: number = 0;

// 字符串
let xxName: string = "xiao fei";
let xxage: number = 18;
let xxIntroduce: string = `我叫${ xxName }, 今年${ xxage }岁`;
let ssName: string = "xiao fei";
let ssage: number = xxage;
let ssIntroduce: string = `我叫${ xxName.toUpperCase() }, 今年${ xxage }岁`;
console.log(ssIntroduce);

// 数组
let numArray: number[] = [1, 2, 3];
console.log(numArray[0]);
numArray[0] = 100;
console.log(numArray[0]);
let strArray: Array<string> = ["1x", "2x", "3x"];
for(let a in strArray) {
    console.log(a);
}
for(let a of strArray) {
    console.log(a);
}

// 元组
let tup:[string, string, number, boolean];
tup = ["hello", "typescript", 1, true];
console.log(tup[0]);

// 枚举
enum Color {
    Red,
    Green,
    Blue
};
let col: Color = Color.Blue;

// 类型断言/转换
let someVal: any = "123xxx";
let slength: number = (<string>someVal).length;
console.log(slength);
slength = (someVal as string).length;
console.log(slength);

// void 类型与 any相反表示没有任何类型，通常作为函数返回值类型
let voidVal: void = undefined;

// null undefined 即使类型又是值
let nullVal: null = null;
let undefinedVal: undefined = undefined;

// 返回never的函数必须存在无法达到的终点 通常作为函数返回值类型
function error(message: string): never {
    throw new Error(message);
}
// try{
//     error("never");
// } catch(e){
//     console.log(e);
// }
//

// 函数
function add(x:number, y:number):number{
    return x + y;
}

let add2:(a:number, b:number)=>number = add;

let add3:(a: number, b: number)=> number = function(x: number, y: number): number {
    return x + y;
}

let add1:(a: number, b: number)=> number = (x: number, y: number) => {
    return x + y;
}
add2(3, 4);
// 函数参数默认值 默认参数必须放在最后
function add4(a: number, b: number = 1): number {
    return a + b;
}
add4(1);
add4(1,2);
// 函数可选参数 需要对可选参数进行判断，没有则为undefined 可选参数必须放在最后
function add5(a: number, b?: number): number {
    if(b) {
        return a + b;
    }
    return a;
}
// 默认参数和可选参数类型相同 add4 和add5 均为
let add6:(a: number, b?: number)=> number = add4;
add6 = add5;
// b 到底是1 还是undefined，不要有二义性
// function add7(a:number, b?:number = 1):number {
//     return b;
// }
