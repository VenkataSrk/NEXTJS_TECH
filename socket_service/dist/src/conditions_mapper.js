"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.conditionsMapper = void 0;
const conditionsMapper = (leftOperand, condition, rightOperand) => {
    let left;
    let right;
    const leftType = isNaN(Number(leftOperand));
    const rightType = isNaN(Number(rightOperand));
    if (!leftType && !rightType) {
        left = Number(leftOperand);
        right = Number(rightOperand);
    }
    else {
        left = leftOperand;
        right = rightOperand;
    }
    const mappedCondition = conditionalOperator[condition];
    switch (mappedCondition) {
        case 'Equal to': {
            return left === right;
        }
        case 'Greater than': {
            return left > right;
        }
        case 'Greater than or Equal to': {
            return left >= right;
        }
        case 'Less than': {
            return left < right;
        }
        case 'Less than or Equal to': {
            return left <= right;
        }
        case 'Starts with': {
            left = left.toString();
            right = right.toString();
            return (left === null || left === void 0 ? void 0 : left.at(0)) === right;
        }
        case 'Ends with': {
            left = left.toString();
            right = right.toString();
            return (left === null || left === void 0 ? void 0 : left.at(-1)) === right;
        }
        case 'Contains': {
            left = left.toString();
            right = right.toString();
            return left === null || left === void 0 ? void 0 : left.includes(right);
        }
        default: {
            return false;
        }
    }
};
exports.conditionsMapper = conditionsMapper;
const conditionalOperator = {
    1: 'Equal to',
    2: 'Greater than',
    3: 'Greater than or Equal to',
    4: 'Less than',
    5: 'Less than or Equal to',
    6: 'Starts with',
    7: 'Ends with',
    8: 'Contains',
};
//# sourceMappingURL=conditions_mapper.js.map